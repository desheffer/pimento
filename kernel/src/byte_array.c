#include <byte_array.h>
#include <list.h>
#include <page.h>
#include <pimento.h>

/**
 * Allocate a new page for the given byte array.
 */
static struct page * _append_page(struct byte_array * byte_array)
{
    struct page * page = page_alloc();

    list_push_back(byte_array->pages, page);

    return page;
}

/**
 * Append the given data to the end of a byte array.
 */
size_t byte_array_append(struct byte_array * byte_array, const char * buf,
                         size_t num)
{
    unsigned p_size = page_size();
    struct page * page = 0;

    char * p_buf = 0;
    unsigned b_idx = 0;
    unsigned p_idx = byte_array->length % p_size;

    // Start on the last page, if one exists.
    if (byte_array->pages->back) {
        page = byte_array->pages->back->item;
        p_buf = page->vaddr;
    }

    while (num-- > 0) {
        // An index of 0 means allocate a new page.
        if (p_idx == 0) {
            page = _append_page(byte_array);
            p_buf = page->vaddr;
        }

        p_buf[p_idx++] = buf[b_idx++];
        p_idx = p_idx % p_size;
    }

    byte_array->length += b_idx;

    return b_idx;
}

/**
 * Copy data out of a byte array.
 */
size_t byte_array_copy(struct byte_array * byte_array, char * buf, size_t num,
                       size_t off)
{
    unsigned p_size = page_size();
    struct list_item * list_item = 0;
    struct page * page = 0;

    char * p_buf = 0;
    unsigned b_idx = 0;
    unsigned p_idx = off % p_size;

    list_item = byte_array->pages->front;
    page = list_item->item;
    p_buf = page->vaddr;

    // Find the starting page.
    while (off >= p_size) {
        off -= p_size;

        list_item = list_item->next;
        page = list_item->item;
        p_buf = page->vaddr;

        if (page == 0) {
            return 0;
        }
    }

    while (num-- > 0) {
        buf[b_idx++] = p_buf[p_idx++];
        p_idx = p_idx % p_size;

        // An index of 0 means traverse to the next page.
        if (p_idx == 0) {
            list_item = list_item->next;
            page = list_item->item;
            p_buf = page->vaddr;
        }

        if (page == 0) {
            break;
        }
    }

    return b_idx;
}

/**
 * Create a byte array.
 */
struct byte_array * byte_array_create(void)
{
    struct byte_array * byte_array = kcalloc(sizeof(byte_array));
    byte_array->pages = list_create();

    return byte_array;
}

/**
 * Destroy a byte array.
 */
void byte_array_destroy(struct byte_array * byte_array)
{
    list_foreach(byte_array->pages, struct page *, page) {
        page_free(page);
    };

    list_destroy(byte_array->pages);
    kfree(byte_array);
}

/**
 * Return the length of a byte array.
 */
size_t byte_array_length(struct byte_array * byte_array)
{
    return byte_array->length;
}
