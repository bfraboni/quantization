#ifndef QUANTIZER_H
#define QUANTIZER_H

#include "image.h"
 
#define ON_INHEAP   1

typedef struct oct_node_t oct_node_t, *oct_node;
struct oct_node_t{
    int64_t r, g, b; /* sum of all child node colors */
    int count, heap_idx;
    unsigned char n_kids, kid_idx, flags, depth;
    oct_node kids[8], parent;
};
 
typedef struct {
    int alloc, n;
    oct_node* buf;
} node_heap;
 
inline int cmp_node(oct_node a, oct_node b)
{
    if (a->n_kids < b->n_kids) return -1;
    if (a->n_kids > b->n_kids) return 1;
 
    int ac = a->count >> a->depth;
    int bc = b->count >> b->depth;
    return ac < bc ? -1 : ac > bc;
}
 
void down_heap(node_heap *h, oct_node p)
{
    int n = p->heap_idx, m;
    while (1) {
        m = n * 2;
        if (m >= h->n) break;
        if (m + 1 < h->n && cmp_node(h->buf[m], h->buf[m + 1]) > 0) m++;
 
        if (cmp_node(p, h->buf[m]) <= 0) break;
 
        h->buf[n] = h->buf[m];
        h->buf[n]->heap_idx = n;
        n = m;
    }
    h->buf[n] = p;
    p->heap_idx = n;
}
 
void up_heap(node_heap *h, oct_node p)
{
    int n = p->heap_idx;
    oct_node prev;
 
    while (n > 1) {
        prev = h->buf[n / 2];
        if (cmp_node(p, prev) >= 0) break;
 
        h->buf[n] = prev;
        prev->heap_idx = n;
        n /= 2;
    }
    h->buf[n] = p;
    p->heap_idx = n;
}
 
void heap_add(node_heap *h, oct_node p)
{
    if ((p->flags & ON_INHEAP)) {
        down_heap(h, p);
        up_heap(h, p);
        return;
    }
 
    p->flags |= ON_INHEAP;
    if (!h->n) h->n = 1;
    if (h->n >= h->alloc) {
        while (h->n >= h->alloc) h->alloc += 1024;
        h->buf = (oct_node_t**) realloc(h->buf, sizeof(oct_node) * h->alloc);
    }
 
    p->heap_idx = h->n;
    h->buf[h->n++] = p;
    up_heap(h, p);
}
 
oct_node pop_heap(node_heap *h)
{
    if (h->n <= 1) return 0;
 
    oct_node ret = h->buf[1];
    h->buf[1] = h->buf[--h->n];
 
    h->buf[h->n] = 0;
 
    h->buf[1]->heap_idx = 1;
    down_heap(h, h->buf[1]);
 
    return ret;
}
 
static oct_node pool = 0;
oct_node node_new(unsigned char idx, unsigned char depth, oct_node p)
{
    static int len = 0;
    if (len <= 1) {
        oct_node p = (oct_node_t*)calloc(sizeof(oct_node_t), 2048);
        p->parent = pool;
        pool = p;
        len = 2047;
    }
 
    oct_node x = pool + len--;
    x->kid_idx = idx;
    x->depth = depth;
    x->parent = p;
    if (p) p->n_kids++;
    return x;
}
 
void node_free()
{
    oct_node p;
    while (pool) {
        p = pool->parent;
        free(pool);
        pool = p;
    }
}
 
oct_node node_insert(oct_node root, unsigned char *pix)
{
    unsigned char i, bit, depth = 0;
 
    for (bit = 1 << 7; ++depth < 8; bit >>= 1) {
        i = !!(pix[1] & bit) * 4 + !!(pix[0] & bit) * 2 + !!(pix[2] & bit);
        if (!root->kids[i])
            root->kids[i] = node_new(i, depth, root);
 
        root = root->kids[i];
    }
 
    root->r += pix[0];
    root->g += pix[1];
    root->b += pix[2];
    root->count++;
    return root;
}
 
oct_node node_fold(oct_node p)
{
    if (p->n_kids) abort();
    oct_node q = p->parent;
    q->count += p->count;
 
    q->r += p->r;
    q->g += p->g;
    q->b += p->b;
    q->n_kids --;
    q->kids[p->kid_idx] = 0;
    return q;
}
 
/* traverse the octree just like construction, but this time we replace the pixel
   color with color stored in the tree node */
void color_replace(oct_node root, unsigned char *pix)
{
    unsigned char i, bit;
 
    for (bit = 1 << 7; bit; bit >>= 1) {
        i = !!(pix[1] & bit) * 4 + !!(pix[0] & bit) * 2 + !!(pix[2] & bit);
        if (!root->kids[i]) break;
        root = root->kids[i];
    }
 
    pix[0] = root->r;
    pix[1] = root->g;
    pix[2] = root->b;
}
 
/* Building an octree and keep leaf nodes in a bin heap.  Afterwards remove first node
   in heap and fold it into its parent node (which may now be added to heap), until heap
   contains required number of colors. */
Image color_quant(const Image& image, int n_colors)
{
    unsigned char * pix = new unsigned char[3 * image.size()];
    #pragma omp for
    for(int i = 0; i < (int)image.size(); ++i)
    {
        pix[3*i+0] = (unsigned char)(image(i).r * 255);
        pix[3*i+1] = (unsigned char)(image(i).g * 255);
        pix[3*i+2] = (unsigned char)(image(i).b * 255);
    }

    node_heap heap = { 0, 0, 0 };
 
    oct_node root = node_new(0, 0, 0), got;
    unsigned char * ptr = pix;
    for (int i = 0; i < (int)image.size(); i++, ptr += 3)
        heap_add(&heap, node_insert(root, ptr));
 
    while (heap.n > n_colors + 1)
        heap_add(&heap, node_fold(pop_heap(&heap)));
 
    for (int i = 1; i < heap.n; i++) 
    {
        got = heap.buf[i];
        float c = got->count;
        got->r = got->r / c + .5;
        got->g = got->g / c + .5;
        got->b = got->b / c + .5;
        // printf("%2d | %3llu %3llu %3llu (%d pixels)\n",
            // i, got->r, got->g, got->b, got->count);
    }
    
    ptr = pix;
    Image out(image.width(), image.height());
    for(int i = 0; i < (int)image.size(); ++i, ptr+=3)
    {
        color_replace(root, ptr);
        out(i) = Color(ptr[0] / 255.f, ptr[1] / 255.f, ptr[2] / 255.f);
    }

    delete[] pix;
    node_free();
    free(heap.buf);

    return out;
}

#endif