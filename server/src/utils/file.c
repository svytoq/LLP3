#include "../../include/utils/file.h"

# define file_size(size, fl){\
    fseek(fl, 0, SEEK_END);\
    size = ftell(fl);\
}

void *read_file(FILE *file, uint64_t offset, uint64_t length) {
    fseek(file, offset, SEEK_SET);
    void *buffer = malloc(length);
    fread(buffer, 1, length, file);
    return buffer;
}


uint64_t try_to_write_in_hole(FILE *file, void *data_ptr, uint64_t size_of_data) {
    struct header *file_header = read_file(file, 0, sizeof(struct header));
    uint64_t hole_ptr = file_header->first_hole_ptr;

    uint64_t save_first_hole_ptr = file_header->first_hole_ptr;
    struct hole *iter_hole;
    while (hole_ptr != INVALID_PTR) {
        iter_hole = read_file(file, hole_ptr, sizeof(struct hole));

        if (iter_hole->size_of_hole == size_of_data) {
            if (hole_ptr != iter_hole->hole_ptr) {

                uint64_t save_ptr = iter_hole->hole_ptr;

                fseek(file, iter_hole->hole_ptr, SEEK_SET);
                fwrite(data_ptr, 1, size_of_data, file);

                iter_hole->size_of_hole = sizeof(struct hole);
                iter_hole->hole_ptr = hole_ptr;


                fseek(file, hole_ptr, SEEK_SET);
                fwrite(iter_hole, 1, sizeof(struct hole), file);

                free(iter_hole);
                return save_ptr;

            } else {

                if (hole_ptr == save_first_hole_ptr) {
                    file_header = read_file(file, 0, sizeof(struct header));
                    file_header->first_hole_ptr = iter_hole->next_ptr;

                    fseek(file, 0, SEEK_SET);
                    fwrite(file_header, 1, sizeof(struct header), file);
                    free(file_header);

                }

                uint64_t save_ptr = iter_hole->hole_ptr;

                if (iter_hole->next_ptr != INVALID_PTR) {
                    struct hole *next_hole = read_file(file, iter_hole->next_ptr, sizeof(struct hole));
                    next_hole->prev_ptr = iter_hole->prev_ptr;

                    fseek(file, iter_hole->next_ptr, SEEK_SET);
                    fwrite(next_hole, 1, sizeof(struct hole), file);

                    free(next_hole);
                }

                if (iter_hole->prev_ptr != INVALID_PTR) {
                    struct hole *prev_hole = read_file(file, iter_hole->prev_ptr, sizeof(struct hole));
                    prev_hole->next_ptr = iter_hole->next_ptr;

                    fseek(file, iter_hole->prev_ptr, SEEK_SET);
                    fwrite(prev_hole, 1, sizeof(struct hole), file);

                    free(prev_hole);
                }

                fseek(file, iter_hole->hole_ptr, SEEK_SET);
                fwrite(data_ptr, 1, size_of_data, file);
                free(iter_hole);

                return save_ptr;
            }
        } else {
            if (iter_hole->size_of_hole > size_of_data) {

                uint64_t save_ptr = iter_hole->hole_ptr;

                fseek(file, iter_hole->hole_ptr, SEEK_SET);
                fwrite(data_ptr, 1, size_of_data, file);
                iter_hole->hole_ptr += size_of_data;
                iter_hole->size_of_hole -= size_of_data;

                if (iter_hole->prev_ptr != INVALID_PTR) {
                    struct hole *previous_hole = read_file(file, iter_hole->prev_ptr, sizeof(struct hole));
                    previous_hole->next_ptr = iter_hole->next_ptr;

                    fseek(file, previous_hole->hole_ptr, SEEK_SET);
                    fwrite(previous_hole, 1, sizeof(struct hole), file);
                }

                if (iter_hole->next_ptr != INVALID_PTR) {
                    struct hole *nextt_hole = read_file(file, iter_hole->next_ptr, sizeof(struct hole));
                    nextt_hole->prev_ptr = iter_hole->prev_ptr;

                    fseek(file, nextt_hole->hole_ptr, SEEK_SET);
                    fwrite(nextt_hole, 1, sizeof(struct hole), file);
                }

                if (file_header->first_hole_ptr == iter_hole->hole_ptr) {
                    if (iter_hole->next_ptr == INVALID_PTR) {
                        return save_ptr;
                    } else {
                        struct hole *current_hole = read_file(file, file_header->first_hole_ptr, sizeof(struct hole));
                        uint64_t address;
                        while (current_hole->size_of_hole > iter_hole->size_of_hole) {
                            address = current_hole->next_ptr;
                            free(current_hole);
                            current_hole = read_file(file, address, sizeof(struct hole));
                        }
                        iter_hole->next_ptr = current_hole->hole_ptr;
                        iter_hole->prev_ptr = current_hole->prev_ptr;
                        current_hole->prev_ptr = iter_hole->hole_ptr;

                        if (current_hole->prev_ptr) {
                            struct hole *before_current_hole = read_file(file, current_hole->prev_ptr,
                                                                         sizeof(struct hole));
                            before_current_hole->next_ptr = iter_hole->hole_ptr;

                            fseek(file, before_current_hole->hole_ptr, SEEK_SET);
                            fwrite(before_current_hole, 1, sizeof(struct hole), file);
                            free(before_current_hole);
                        } else {
                            file_header->first_hole_ptr = iter_hole->hole_ptr;
                            fseek(file, iter_hole->hole_ptr, SEEK_SET);
                            fwrite(iter_hole, 1, sizeof(struct hole), file);

                            fseek(file, 0, SEEK_SET);
                            fwrite(file_header, 1, sizeof(struct header), file);
                        }
                    }
                }
                return save_ptr;
            }
        }
        free(iter_hole);
        free(file_header);
        break;
    }
    return INVALID_PTR;
}

uint64_t write_file(FILE *file, void *data_ptr, uint64_t size_of_data) {
    uint64_t size;
    file_size(size, file);

    uint64_t return_try_hole = try_to_write_in_hole(file, data_ptr, size_of_data);

    if (return_try_hole != INVALID_PTR) {
        return return_try_hole;
    }

    fseek(file, 0, SEEK_END);
    fwrite(data_ptr, 1, size_of_data, file);
    return size;
}


FILE *open_file(char *name) {

    FILE *f = fopen(name, "r+");

    if (f == NULL) {
        return NULL;
    }

    uint64_t size_of_file;
    file_size(size_of_file, f);
    if (size_of_file < sizeof(struct header)) {
        ftrunc(fileno(f), sizeof(struct header));
    }

    fseek(f, 0, SEEK_SET);
    struct header *file_header = (struct header *) malloc(sizeof(struct header));
    fread(file_header, 1, sizeof(struct header), f);

    uint32_t signature = file_header->signature;

    if (signature == 0xDEADDEAD) {
        return f;
    }

    file_header->signature = 0xDEADDEAD;
    file_header->first_hole_ptr = INVALID_PTR;
    file_header->first_node_ptr = INVALID_PTR;
    file_header->node_id = 0;
    file_header->last_node_ptr = INVALID_PTR;

    ftrunc(fileno(f), sizeof(struct header));
    fseek(f, 0, SEEK_SET);
    fwrite(file_header, 1, sizeof(struct header), f);
    free(file_header);
    return f;
}


void delete_from_file(FILE *file, uint64_t offset, uint64_t length) {
    struct header *file_header = read_file(file, 0, sizeof(struct header));
    if (file_header->first_hole_ptr == INVALID_PTR) {
        struct hole first_real_hole = (struct hole) {hole_ptr: offset, size_of_hole:length, prev_ptr:INVALID_PTR, next_ptr:INVALID_PTR};
        file_header->first_hole_ptr = write_file(file, &first_real_hole, sizeof(struct hole));
        fseek(file, 0, SEEK_SET);
        fwrite(file_header, 1, sizeof(file_header), file);
        free(file_header);
        return;
    }

    struct hole new_hole = (struct hole) {
            hole_ptr: offset,
            size_of_hole:length,
            prev_ptr:INVALID_PTR,
            next_ptr:file_header->first_hole_ptr
    };

    struct hole *iter_hole = read_file(file, file_header->first_hole_ptr, sizeof(struct hole));
    uint64_t address;

    while ((iter_hole->size_of_hole > new_hole.size_of_hole) && iter_hole->next_ptr != INVALID_PTR) {
        address = iter_hole->next_ptr;
        free(iter_hole);
        iter_hole = read_file(file, address, sizeof(struct hole));

    }

    new_hole.prev_ptr = iter_hole->prev_ptr;
    new_hole.next_ptr = address;
    if (iter_hole->prev_ptr) {
        struct hole *prev_hole = read_file(file, iter_hole->prev_ptr, sizeof(struct hole));
        prev_hole->next_ptr = write_file(file, &new_hole, sizeof(struct hole));

        iter_hole->prev_ptr = prev_hole->next_ptr;

        fseek(file, prev_hole->next_ptr, SEEK_SET);
        fwrite(&new_hole, 1, sizeof(struct hole), file);

        fseek(file, iter_hole->prev_ptr, SEEK_SET);
        fwrite(&new_hole, 1, sizeof(struct hole), file);

        free(prev_hole);

    } else {

        iter_hole->prev_ptr = write_file(file, &new_hole, sizeof(struct hole));
        fseek(file, iter_hole->prev_ptr, SEEK_SET);
        fwrite(&new_hole, 1, sizeof(struct hole), file);


        file_header->first_hole_ptr = write_file(file, &new_hole, sizeof(struct hole));

        fseek(file, 0, SEEK_SET);
        fwrite(file_header, 1, sizeof(file_header), file);
    }

    free(iter_hole);
    free(file_header);

    return;
}
