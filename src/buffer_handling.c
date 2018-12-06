/**********************************************************************
 *
 * TilelessMap
 *
 * TilelessMap is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * TilelessMap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TilelessMap.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************
 *
 * Copyright (C) 2016-2018 Nicklas Avén
 *
 ***********************************************************************/


#include "theclient.h"
#include "mem.h"
#include "buffer_handling.h"
#include "uthash.h"
#include "read_data/twkb.h"





/************* GL Float List ********************/


GLFLOAT_LIST* init_glfloat_list()
{
    GLFLOAT_LIST *res = (GLFLOAT_LIST*) st_malloc(sizeof(GLFLOAT_LIST));

    res->list = st_malloc(INIT_LIST_SIZE * sizeof(GLfloat));
    res->alloced = INIT_LIST_SIZE;
    res->used = 0;

    return res;
}



static int increase_glfloat_list(GLFLOAT_LIST *l, size_t needed_space)
{

    size_t available_space = l->alloced-l->used;

    if (available_space >= needed_space)
        return 0;


    size_t new_size = l->alloced;

    while (available_space < needed_space)
    {
        new_size*=2;
        available_space = new_size - l->used;
    }
    l->list = st_realloc(l->list, new_size * sizeof(GLfloat));
    l->alloced = new_size;
    return 0;
}

int reset_glfloat_list(GLFLOAT_LIST *l)
{
    l->used = 0;
    return 0;
}


int destroy_glfloat_list(GLFLOAT_LIST *l)
{
    free(l->list);
    l->list = NULL;
    l->used = 0;
    l->alloced = 0;
    free(l);
    l = NULL;
    return 0;
}

int add2glfloat_list(GLFLOAT_LIST *list, GLfloat val)
{
    increase_glfloat_list(list, 1);
    *(list->list + list->used) = val;
    (list->used)++;
    return 0;
}



int addbatch2glfloat_list(GLFLOAT_LIST *list,GLuint n_vals, GLfloat *vals)
{
    increase_glfloat_list(list, n_vals);
    memcpy(list->list + list->used, vals, n_vals * sizeof(GLfloat));
    list->used += n_vals;
    return 0;
}



/************* GLUInt List ********************/
GLUINT_LIST* init_gluint_list()
{
    GLUINT_LIST *res = (GLUINT_LIST*) st_malloc(sizeof(GLUINT_LIST));

    res->list = st_malloc(INIT_LIST_SIZE * sizeof(GLuint));
    res->alloced = INIT_LIST_SIZE;
    res->used = 0;

    return res;
}



static int increase_gluint_list(GLUINT_LIST *l, size_t needed_space)
{

    size_t available_space = l->alloced-l->used;

    if (available_space >= needed_space)
        return 0;


    size_t new_size = l->alloced;

    while (available_space < needed_space)
    {
        new_size*=2;
        available_space = new_size - l->used;
    }
    l->list = st_realloc(l->list, new_size * sizeof(GLuint));
    l->alloced = new_size;
    return 0;
}

int reset_gluint_list(GLUINT_LIST *l)
{
    l->used = 0;
    return 0;
}


int destroy_gluint_list(GLUINT_LIST *l)
{
    free(l->list);
    l->list = NULL;
    l->used = 0;
    l->alloced = 0;
    free(l);
    l = NULL;
    return 0;
}

int add2gluint_list(GLUINT_LIST *list, GLuint val)
{
    increase_gluint_list(list, 1);
    *(list->list + list->used) = val;
    list->used++;
    return 0;
}

int addbatch2gluint_list(GLUINT_LIST *list,GLuint n_vals, GLuint *vals)
{
    increase_gluint_list(list, n_vals);
    memcpy(list->list + list->used, vals, n_vals * sizeof(GLuint));
    list->used += n_vals;
    return 0;
}


/************* int64 List ********************/
static INT64_LIST* init_int64_list()
{
    INT64_LIST *res = (INT64_LIST*) st_malloc(sizeof(INT64_LIST));

    res->list = st_malloc(INIT_LIST_SIZE * sizeof(int64_t));
    res->alloced = INIT_LIST_SIZE;
    res->used = 0;

    return res;
}



static int increase_int64_list(INT64_LIST *l, size_t needed_space)
{

    size_t available_space = l->alloced-l->used;

    if (available_space >= needed_space)
        return 0;


    size_t new_size = l->alloced;

    while (available_space < needed_space)
    {
        new_size*=2;
        available_space = new_size - l->used;
    }
    l->list = st_realloc(l->list, new_size * sizeof(int64_t));
    l->alloced = new_size;
    return 0;
}

static int reset_int64_list(INT64_LIST *l)
{
    l->used = 0;
    return 0;
}


int destroy_int64_list(INT64_LIST *l)
{
    free(l->list);
    l->list = NULL;
    l->used = 0;
    l->alloced = 0;
    free(l);
    l = NULL;
    return 0;
}

int add2int64_list(INT64_LIST *list, int64_t val)
{
    increase_int64_list(list, 1);
    *(list->list + list->used) = val;
    list->used++;
    return 0;
}

int addbatch2int64_list(INT64_LIST *list,GLuint n_vals, int64_t *vals)
{
    increase_int64_list(list, n_vals);
    memcpy(list->list + list->used, vals, n_vals * sizeof(int64_t));
    list->used += n_vals;
    return 0;
}

int setzero2int64_list(INT64_LIST *list,int64_t n_vals)
{
    increase_int64_list(list, n_vals);
    memset(list->list + list->used, 0, n_vals * sizeof(int64_t));
    list->used += n_vals;
    return 0;
}

/************* GL UShort List ********************/
GLUSHORT_LIST* init_glushort_list()
{
    GLUSHORT_LIST *res = (GLUSHORT_LIST*) st_malloc(sizeof(GLUSHORT_LIST));

    res->list = st_malloc(INIT_LIST_SIZE * sizeof(GLushort));
    res->alloced = INIT_LIST_SIZE;
    res->used = 0;

    return res;
}



static int increase_glushort_list(GLUSHORT_LIST *l, size_t needed_space)
{

    size_t available_space = l->alloced-l->used;

    if (available_space >= needed_space)
        return 0;


    size_t new_size = l->alloced;

    while (available_space < needed_space)
    {
        new_size*=2;
        available_space = new_size - l->used;
    }
    l->list = st_realloc(l->list, new_size * sizeof(GLushort));
    l->alloced = new_size;
    return 0;
}

static int reset_glushort_list(GLUSHORT_LIST *l)
{
    l->used = 0;
    return 0;
}


int destroy_glushort_list(GLUSHORT_LIST *l)
{
    free(l->list);
    l->list = NULL;
    l->used = 0;
    l->alloced = 0;
    free(l);
    l = NULL;
    return 0;
}

int add2glushort_list(GLUSHORT_LIST *list, GLshort val)
{
    increase_glushort_list(list, 1);
    *(list->list + list->used) = val;
    list->used++;
    return 0;
}


int addbatch2glushort_list(GLUSHORT_LIST *list,GLuint n_vals, GLushort *vals)
{
    increase_glushort_list(list, n_vals);
    memcpy(list->list + list->used, vals, n_vals * sizeof(GLushort));
    list->used += n_vals;
    return 0;
}

/************* UINT8_LIST List ********************/
UINT8_LIST* init_uint8_list()
{
    UINT8_LIST *res = (UINT8_LIST*) st_malloc(sizeof(UINT8_LIST));

    res->list = st_malloc(INIT_LIST_SIZE * sizeof(uint8_t));
    res->alloced = INIT_LIST_SIZE;
    res->used = 0;

    return res;
}



static int increase_uint8_list(UINT8_LIST *l, size_t needed_space)
{

    size_t available_space = l->alloced-l->used;

    if (available_space >= needed_space)
        return 0;


    size_t new_size = l->alloced;

    while (available_space < needed_space)
    {
        new_size*=2;
        available_space = new_size - l->used;
    }
    l->list = st_realloc(l->list, new_size * sizeof(uint8_t));
    l->alloced = new_size;
    return 0;
}

int reset_uint8_list(UINT8_LIST *l)
{
    l->used = 0;
    return 0;
}


int destroy_uint8_list(UINT8_LIST *l)
{
    free(l->list);
    l->list = NULL;
    l->used = 0;
    l->alloced = 0;
    free(l);
    l = NULL;
    return 0;
}

int add2uint8_list(UINT8_LIST *list, uint8_t val)
{
    increase_uint8_list(list, 1);
    *(list->list + list->used) = val;
    list->used++;
    return 0;
}


int addbatch2uint8_list(UINT8_LIST *list,GLuint n_vals, uint8_t *vals)
{
    increase_uint8_list(list, n_vals);
    memcpy(list->list + list->used, vals, n_vals * sizeof(uint8_t));
    list->used += n_vals;
    return 0;
}


/************* POINTER_LIST List ********************/
POINTER_LIST* init_pointer_list()
{
    POINTER_LIST *res = (POINTER_LIST*) st_malloc(sizeof(POINTER_LIST));

    res->list = st_malloc(INIT_LIST_SIZE * sizeof(void*));
    res->alloced = INIT_LIST_SIZE;
    res->used = 0;

    return res;
}



static int increase_pointer_list(POINTER_LIST *l, size_t needed_space)
{

    size_t available_space = l->alloced-l->used;

    if (available_space >= needed_space)
        return 0;


    size_t new_size = l->alloced;

    while (available_space < needed_space)
    {
        new_size*=2;
        available_space = new_size - l->used;
    }
    l->list = st_realloc(l->list, new_size * sizeof(void*));
    l->alloced = new_size;
    return 0;
}

int reset_pointer_list(POINTER_LIST *l)
{
    l->used = 0;
    return 0;
}


int destroy_pointer_list(POINTER_LIST *l)
{
    free(l->list);
    l->list = NULL;
    l->used = 0;
    l->alloced = 0;
    free(l);
    l = NULL;
    return 0;
}

int add2pointer_list(POINTER_LIST *list, void *val)
{
    increase_pointer_list(list, 1);
    //memcpy(list->list + list->used, val,sizeof(void*));
    *(list->list + list->used) = val;
    list->used++;
    return 0;
}


int addbatch2pointer_list(POINTER_LIST *list,GLuint n_vals, void *vals)
{
    increase_pointer_list(list, n_vals);
    memcpy(list->list + list->used, vals, n_vals * sizeof(void*));
    list->used += n_vals;
    return 0;
}


int setzero2pointer_list(POINTER_LIST *list,GLuint n_vals)
{
    increase_pointer_list(list, n_vals);
    memset(list->list + list->used, 0, n_vals * sizeof(void*));
    list->used += n_vals;
    return 0;
}




static RASTER_LIST* init_raster_list()
{
    RASTER_LIST *res = st_malloc(sizeof(RASTER_LIST));
    res->data = init_uint8_list();
    res->raster_start_indexes = init_gluint_list();
    res->tileidxy = init_gluint_list();
    glGenBuffers(1, &(res->cvbo));
    glGenBuffers(1, &(res->cibo));
    glGenBuffers(1, &(res->vbo));
    return res;
}
static POINT_LIST* init_point_list()
{
    POINT_LIST *res = st_malloc(sizeof(POINT_LIST));
    res->points = init_glfloat_list();
    res->style_id = init_pointer_list();
    res->point_start_indexes = init_gluint_list();
    glGenBuffers(1, &(res->vbo));
    glGenBuffers(1, &(res->tbo)); //For text
    return res;
}

static LINESTRING_LIST* init_linestring_list()
{
    LINESTRING_LIST *res = st_malloc(sizeof(LINESTRING_LIST));
    res->vertex_array = init_glfloat_list();
    res->line_start_indexes = init_gluint_list();
    res->style_id = init_pointer_list();
    glGenBuffers(1, &(res->vbo));

    return res;
}

static POLYGON_LIST* init_polygon_list()
{
    POLYGON_LIST *res = st_malloc(sizeof(POLYGON_LIST));
    res->vertex_array = init_glfloat_list();
    res->pa_start_indexes = init_gluint_list();
    res->polygon_start_indexes = init_gluint_list();
    res->element_array = init_glushort_list();
    res->element_start_indexes = init_gluint_list();
    res->style_id = init_pointer_list();
    res->line_style_id = init_pointer_list();

    glGenBuffers(1, &(res->vbo));
    glGenBuffers(1, &(res->ebo));
    return res;
}



static int reset_raster_list(RASTER_LIST *l)
{
    if(!l)
        return 0;
    reset_gluint_list(l->tileidxy);
    reset_gluint_list(l->raster_start_indexes);
    reset_uint8_list(l->data);
    return 0;
}
int reset_point_list(POINT_LIST *l)
{
    if(!l)
        return 0;
    reset_glfloat_list(l->points);
    reset_gluint_list(l->point_start_indexes);
    reset_pointer_list(l->style_id);
    return 0;
}

static int reset_linestring_list(LINESTRING_LIST *l)
{
    if(!l)
        return 0;
    reset_glfloat_list(l->vertex_array);
    reset_gluint_list(l->line_start_indexes);
    reset_pointer_list(l->style_id);

    return 0;
}

static int reset_polygon_list(POLYGON_LIST *l)
{
    if(!l)
        return 0;
    reset_glfloat_list(l->vertex_array);
    reset_gluint_list(l->pa_start_indexes);
    reset_gluint_list(l->polygon_start_indexes);
    reset_glushort_list(l->element_array);
    reset_gluint_list(l->element_start_indexes);
    reset_pointer_list(l->style_id);
    reset_pointer_list(l->line_style_id);
    return 0;
}


static int destroy_raster_list(RASTER_LIST *l)
{
    if(!l)
        return 0;
    destroy_gluint_list(l->tileidxy);
    destroy_gluint_list(l->raster_start_indexes);
    destroy_uint8_list(l->data);
    glDeleteBuffers(1,&(l->vbo));
    glDeleteBuffers(1,&(l->cibo));
    glDeleteBuffers(1,&(l->cvbo));
    free(l);
    l=NULL;
    return 0;
}
int destroy_point_list(POINT_LIST *l)
{
    if(!l)
        return 0;
    destroy_glfloat_list(l->points);
    destroy_gluint_list(l->point_start_indexes);
    destroy_pointer_list(l->style_id);
    glDeleteBuffers(1,&(l->vbo));
    free(l);
    l=NULL;
    return 0;
}

static int destroy_linestring_list(LINESTRING_LIST *l)
{
    if(!l)
        return 0;

    destroy_glfloat_list(l->vertex_array);
    destroy_gluint_list(l->line_start_indexes);
    destroy_pointer_list(l->style_id);
    glDeleteBuffers(1,&(l->vbo));
    free(l);
    l=NULL;
    return 0;

}

static int destroy_polygon_list(POLYGON_LIST *l)
{
    if(!l)
        return 0;
    destroy_glfloat_list(l->vertex_array);
    destroy_gluint_list(l->pa_start_indexes);
    destroy_gluint_list(l->polygon_start_indexes);
    destroy_glushort_list(l->element_array);
    destroy_gluint_list(l->element_start_indexes);
    destroy_pointer_list(l->style_id);
    destroy_pointer_list(l->line_style_id);
    glDeleteBuffers(1,&(l->vbo));
    glDeleteBuffers(1,&(l->ebo));
    free(l);
    return 0;
}







int init_buffers(LAYER_RUNTIME *layer)
{
    if(layer->type & 224)
        layer->points = init_point_list();
    else
        layer->points = NULL;
    if(layer->type & 16)
        layer->lines = init_linestring_list();
    else
        layer->lines = NULL;
    if(layer->type & 8)
        layer->wide_lines = init_linestring_list();
    else
        layer->wide_lines = NULL;
    if (layer->type & 6)
        layer->polygons = init_polygon_list();
    else
        layer->polygons = NULL;

    layer->twkb_id = init_int64_list();

    if(layer->geometryType == RASTER)
        layer->rast = init_raster_list();
    //  layer->style_id = init_gluint_list();
    return 0;
}


int reset_buffers(LAYER_RUNTIME *layer)
{
    if(layer->points)
        reset_point_list(layer->points);
    if(layer->lines)
        reset_linestring_list(layer->lines);
    if(layer->wide_lines)
        reset_linestring_list(layer->wide_lines);
    if(layer->polygons)
        reset_polygon_list(layer->polygons);

    if(layer->twkb_id)
        reset_int64_list(layer->twkb_id);

    if(layer->geometryType == RASTER)
        reset_raster_list(layer->rast);
    //  reset_gluint_list(layer->style_id);
    return 0;
}




struct STYLES* get_style(struct STYLES *styles, void *val,int val_type)
{
    struct STYLES *s = NULL;
    if(val_type == INT_TYPE)
    {
        HASH_FIND_INT( styles, val, s);
        log_this(10,"  val = %d and style is %p\n", *((int*) val), s);
        if(!s)
        {
            int v = -1;
            HASH_FIND_INT(styles, &v, s);
        }
    }
    else if (val_type == STRING_TYPE)
    {

        HASH_FIND_STR(styles, val, s);

        log_this(10," val = %s and style is %p \n",(char*) val, s);
        if(!s)
        {
            HASH_FIND_STR(styles, "-1", s);
        }
    }

    return s;
}

GLFLOAT_LIST* get_coord_list(LAYER_RUNTIME *l, TWKB_PARSE_STATE *ts)
{
    struct STYLES *s;
    log_this(10,"layer = %s  and ",l->name);
//   add2gluint_list(l->style_id, style_id);
    int type = l->type;
    if(type & 224)
    {
//        add2union_list(l->points->style_id, &(ts->styleID));
        s = get_style(l->styles, &(ts->styleID), ts->styleid_type);


        add2pointer_list(l->points->style_id, s);
        return l->points->points;
    }
    else if(type & 16)
    {
        s = get_style(l->styles, &(ts->styleID), ts->styleid_type);
        add2pointer_list(l->lines->style_id, s);
        return l->lines->vertex_array;
    }

    else if(type & 6)
    {
        s = get_style(l->styles,  &(ts->styleID), ts->styleid_type);
        add2pointer_list(l->polygons->line_style_id, s);
        return l->polygons->vertex_array;
    }
    else
        return NULL;



}


GLFLOAT_LIST* get_wide_line_list(LAYER_RUNTIME *l, TWKB_PARSE_STATE *ts)
{
    struct STYLES *s = get_style(l->styles, &(ts->styleID), ts->styleid_type);

    add2pointer_list(l->wide_lines->style_id, s);
//   add2union_list(l->wide_lines->style_id, &(ts->styleID));
    return l->wide_lines->vertex_array;

}

int pa_end(LAYER_RUNTIME *l, int64_t id)
{
    add2int64_list(l->twkb_id, id);

    int type = l->type;
    if(type & 224)
        add2gluint_list(l->points->point_start_indexes, l->points->points->used);
    if(type & 16)
        add2gluint_list(l->lines->line_start_indexes, l->lines->vertex_array->used);
    if(type & 8)
        add2gluint_list(l->wide_lines->line_start_indexes, l->wide_lines->vertex_array->used);
    if(type & 6)
        add2gluint_list(l->polygons->pa_start_indexes, l->polygons->vertex_array->used);


    return 0;
}





int destroy_buffers(LAYER_RUNTIME *layer)

{
    if(layer->type & 224)
        destroy_point_list(layer->points);
    if(layer->type & 16)
        destroy_linestring_list(layer->lines);
    if(layer->type & 8)
        destroy_linestring_list(layer->wide_lines);
    if (layer->type & 4)
        destroy_polygon_list(layer->polygons);

    destroy_int64_list(layer->twkb_id);

    if(layer->geometryType == RASTER)
        destroy_raster_list(layer->rast);
    return 0;
}


SYMBOLS* init_symbol_list()
{
    SYMBOLS *res = st_malloc(sizeof(SYMBOLS));
    res->points = init_point_list();
    return res;
}

POINT_LIST* init_tb_point_list()
{
    return init_point_list();
}

int destroy_symbol_list(SYMBOLS *l)
{
    destroy_point_list(l->points);
    free(l);
    return 0;
}


int addsym(uint8_t id, size_t n_points, GLfloat *points)
{
    if(id<global_symbols->points->point_start_indexes->used)
        return 1;

    //If not all symbol ids exist we have to fill the list with dummy-posts
    while (id>global_symbols->points->point_start_indexes->used)
    {
        add2gluint_list(global_symbols->points->point_start_indexes, global_symbols->points->points->used);
    }

    addbatch2glfloat_list(global_symbols->points->points, n_points, points);
    add2gluint_list(global_symbols->points->point_start_indexes, global_symbols->points->points->used);
    return 0;

}


void text_reset_buffer(TEXTSTRUCT *text_buf)
{
    text_buf->used_n_vals = 0;
    text_buf->used_n_chars = 0;
}

void text_destroy_buffer(TEXTSTRUCT *text_buf)
{
    free(text_buf->char_array);
    free(text_buf->rotation);
    free(text_buf->size);
    free(text_buf->styleID);
    free(text_buf->anchor);
    destroy_textblock(text_buf->tb);
    free(text_buf);
    return;
}


/*
int destroy_symbols()
{
 destroy_point_list(global_symbols->points);
    free(global_styles);
    return 0;
}
*/















