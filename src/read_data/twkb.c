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


#include "../theclient.h"
#include "../buffer_handling.h"
#include "twkb.h"
/*
static int get_blob(TWKB_BUF *tb,sqlite3_stmt *res, int icol)
{


    //twkb-buffer
    uint8_t *buf;
    size_t buf_len;
    const sqlite3_blob *db_blob;

    db_blob = sqlite3_column_blob(res, icol);

    buf_len = sqlite3_column_bytes(res, icol);
    //   log_this(10, "blob size;%d\n", buf_len);
    buf = malloc(buf_len);
    memcpy(buf, db_blob,buf_len);



    tb->start_pos = tb->read_pos=buf;
    tb->end_pos=buf+buf_len;
    //printf("allocate buffer at %p\n",tb->start_pos);
    return 0;

}*/



static int get_blob( sqlite3_stmt *prep, int icol,UINT8_LIST *res)
{


    /*twkb-buffer*/
    uint8_t *buf;
    size_t buf_len;
    const sqlite3_blob *db_blob;

    db_blob = sqlite3_column_blob(prep, icol);

    buf_len = sqlite3_column_bytes(prep, icol);
    //   log_this(10, "blob size;%d\n", buf_len);
 /*   buf = malloc(buf_len);
    memcpy(buf, db_blob,buf_len);
*/
    addbatch2uint8_list(res,buf_len,(uint8_t*) db_blob);
    //printf("allocate buffer at %p\n",tb->start_pos);

    return 0;


}

void *twkb_fromSQLiteBBOX_thread(void *theL)
{
    twkb_fromSQLiteBBOX(theL);

    pthread_exit(NULL);
    return NULL;
}

void *twkb_fromSQLiteBBOX(void *theL)
{
    log_this(10, "Entering twkb_fromSQLiteBBOX, prepared = %p\n", ((LAYER_RUNTIME*) theL)->preparedStatement->ps);
    /*twkb structures*/
    TWKB_HEADER_INFO thi;
    TWKB_PARSE_STATE ts;
    TWKB_BUF tb;
    sqlite3_stmt *prepared_statement;
    UINT8_LIST *res;
    size_t res_len;
    GLfloat *ext;
    BBOX bbox;
    ts.thi = &thi;
    ts.thi->bbox=&bbox;
    LAYER_RUNTIME *theLayer = (LAYER_RUNTIME *) theL;

    ts.theLayer = (LAYER_RUNTIME *) theL;
    res = ts.theLayer->rawdata;
    GLfloat rotation;
    GLint anchor;
    int size;
//log_this(10, "sqlite_error? %d\n",sqlite3_config(SQLITE_CONFIG_SERIALIZED ));


    prepared_statement = theLayer->preparedStatement->ps;
    ext = theLayer->BBOX;
    //rc = sqlite3_exec(db, sql, callback, 0, &err_msg);


    int err = sqlite3_errcode(projectDB);
    if(err)
        log_this(1,"sqlite problem, %d\n",err);

    if((theLayer->utm_zone != curr_utm) || (theLayer->hemisphere != curr_hemi))
    {
        GLfloat reproj_coord[2];
        GLfloat maxx;
        GLfloat maxy;
        GLfloat minx;
        GLfloat miny;




        reproj_coord[0] = ext[0];
        reproj_coord[1] = ext[1];

        reproject(reproj_coord,curr_utm, theLayer->utm_zone,curr_hemi, theLayer->hemisphere);
        minx = reproj_coord[0];
        miny = reproj_coord[1];

        reproj_coord[0] = ext[0];
        reproj_coord[1] = ext[3];
        reproject(reproj_coord,curr_utm, theLayer->utm_zone,curr_hemi, theLayer->hemisphere);

        if(minx>reproj_coord[0])
            minx = reproj_coord[0];

        maxy = reproj_coord[1];


        reproj_coord[0] = ext[2];
        reproj_coord[1] = ext[3];
        reproject(reproj_coord,curr_utm, theLayer->utm_zone,curr_hemi, theLayer->hemisphere);

        if(maxy<reproj_coord[1])
            maxy = reproj_coord[1];

        maxx = reproj_coord[0];

        reproj_coord[0] = ext[2];
        reproj_coord[1] = ext[1];
        reproject(reproj_coord,curr_utm, theLayer->utm_zone,curr_hemi, theLayer->hemisphere);

        if(maxx<reproj_coord[0])
            maxx = reproj_coord[0];

        if(miny > reproj_coord[1])
            miny = reproj_coord[1];


        sqlite3_bind_double(prepared_statement, 1,(float) maxx); //minX
        sqlite3_bind_double(prepared_statement, 2,(float) minx); //minY
        sqlite3_bind_double(prepared_statement, 3,(float) maxy); //maxX
        sqlite3_bind_double(prepared_statement, 4,(float) miny); //maxY


    }
    else
    {


        sqlite3_bind_double(prepared_statement, 1,(float) ext[2]); //maxX
        sqlite3_bind_double(prepared_statement, 2,(float) ext[0]); //minX
        sqlite3_bind_double(prepared_statement, 3,(float) ext[3]); //maxY
        sqlite3_bind_double(prepared_statement, 4,(float) ext[1]); //minY
        log_this(10, "1 = %f, 2 = %f, 3 = %f, 4 = %f\n", ext[2],ext[0],ext[3],ext[1]);

    }



    err = sqlite3_errcode(projectDB);
    if(err)
        log_this(1,"sqlite problem 2, %d\n",err);

    WCHAR_TEXT *unicode_txt;
    if(theLayer->type & 32)
    {
         ts.unicode_txt = NULL ;
         unicode_txt = init_wc_txt(64);
         reset_textblock(theLayer->text->tb);
    }
    /*
    if(theLayer->points)
        theLayer->points->style_id->list_type = theLayer->style_key_type;
    if(theLayer->lines)
        theLayer->lines->style_id->list_type = theLayer->style_key_type;
    if(theLayer->polygons)
        theLayer->polygons->style_id->list_type = theLayer->style_key_type;
    */

    while (sqlite3_step(prepared_statement)==SQLITE_ROW)
    {

        if(theLayer->geometryType == RASTER)
        {

            if(get_blob(prepared_statement,1, res))
            {
                fprintf(stderr, "Failed to select data\n");

                sqlite3_close(projectDB);
                return NULL;
            }
            addbatch2uint8_list(theLayer->rast->data,res->used, res->list);
            add2gluint_list(theLayer->rast->raster_start_indexes, res->used);


            int x = sqlite3_column_int(prepared_statement,5);
            add2gluint_list(theLayer->rast->tileidxy, x);
            int y = sqlite3_column_int(prepared_statement,6);
            add2gluint_list(theLayer->rast->tileidxy, y);


        }
        ts.id = sqlite3_column_int(prepared_statement, 3);
        // printf("id fra db = %ld\n",ts.id);
        ts.styleid_type = theLayer->style_key_type;
        if(ts.styleid_type == INT_TYPE)
        {
            ts.styleID.int_type = sqlite3_column_int(prepared_statement, 4);
        }
        else if(ts.styleid_type == STRING_TYPE)
        {
            const unsigned char *str = sqlite3_column_text(prepared_statement, 4);
            strcpy(ts.styleID.string_type,(const char*) str);
        }
        else
        {
            if(theLayer->geometryType != RASTER)
            {
                log_this(100, "Error, invalid style key type: %d\n", ts.styleid_type);
                return NULL;
            }
        }
        if(get_blob(prepared_statement,0, res))
        {

            log_this(1,"Failed to select data\n");

            sqlite3_close(projectDB);
            return NULL;
        }
        tb.start_pos = tb.read_pos = res->list;
        tb.end_pos=res->list+res->used;
        ts.tb=&tb;
        ts.utm_zone = theLayer->utm_zone;
        ts.hemisphere = theLayer->hemisphere;
        while (ts.tb->read_pos<ts.tb->end_pos)
        {
            decode_twkb(&ts);//, theLayer->res_buf);
        }
        //printf("start free %p, n_pa = %d\n",tb.start_pos, res_buf->used_n_pa);
 //       free(tb.start_pos);
        reset_uint8_list(res);
        if(theLayer->type & 4)
        {
            if(get_blob(prepared_statement,1, res))
            {
                fprintf(stderr, "Failed to select data\n");

                sqlite3_close(projectDB);
                return NULL;
            }
            tb.start_pos = tb.read_pos = res->list;
            tb.end_pos=res->list+res->used;
            ts.tb=&tb;

            while (ts.tb->read_pos<ts.tb->end_pos)
            {
                decode_element_array(&ts);
            }
            //printf("start free %p, n_pa = %d\n",tb.start_pos, res_buf->used_n_pa);
            //free(tb.start_pos);

            reset_uint8_list(res);


        }
        if(theLayer->type & 32)
        {
            const char *txt = (const char*) sqlite3_column_text(prepared_statement, 5);

            size = sqlite3_column_int(prepared_statement, 6);
            rotation = (GLfloat) sqlite3_column_double(prepared_statement, 7);
            anchor = (GLint) sqlite3_column_double(prepared_statement, 8);
            struct STYLES *s = theLayer->points->style_id->list[theLayer->points->style_id->used-1];
            append_2_textblock(theLayer->text->tb, txt, s->text_styles->a->list[0],NULL, 0, NEW_STRING,unicode_txt);
        
            ts.txt = txt;
            //text_write(txt,0, (GLshort) size, rotation,anchor, theLayer->text);
        }

    }
    
    if(theLayer->type & 32)
        destroy_wc_txt(unicode_txt);
    sqlite3_clear_bindings(prepared_statement);
    sqlite3_reset(prepared_statement);

    return NULL;
}





