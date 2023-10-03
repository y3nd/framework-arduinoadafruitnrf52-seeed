#include "user_stored_data.h"
#include "Lbm_packet.hpp"


#define FILENAME    "/group_id.txt"

volatile uint16_t delete_datas_cnt = 0;
volatile bool delete_datas_fifo = false;



uint8_t read_lfs_file( uint16_t file_name, uint8_t *data, uint16_t *len ) 
{
    int ret = 0;
    ret = key_value_fds_value_read( NULL, file_name, data, *len );
    if( ret >= 0 )
    {
        // *len = ret;
        return 0;    
    }
    hal_mcu_trace_print("key_value_fds_value_read error,ret:%02x\r\n",ret);
    return ret;
}

uint8_t write_lfs_file( uint16_t file_name, uint8_t *data, uint16_t len )
{
    int ret = 0;
    ret = key_value_fds_value_write( NULL, file_name, data, len );
    if( ret >= 0 )
    {
        return 0;    
    }
    hal_mcu_trace_print("key_value_fds_value_write error,ret:%02x\r\n",ret);
    return ret;
}

uint8_t read_lfs_file_one( uint16_t file_name, uint8_t *data, uint16_t *len ) 
{
    int ret = 0xff;
    uint16_t file_name_start = 0;
    uint32_t file_name_offset = 0;

    if( file_name >= POS_FILE_ID_START && file_name <= POS_FILE_ID_END )
    {
        // file_name_start = POS_FILE_ID_START;
        // file_name_offset = ( file_name - file_name_start ) * ( *len );
        file_name_start = POS_FILE_ID_START + ( file_name - POS_FILE_ID_START ) / POS_SAVE_IN_FILE_MAX;
        file_name_offset = (( file_name - POS_FILE_ID_START ) % POS_SAVE_IN_FILE_MAX ) * ( *len );
    }
    else
    {
        return ret;
    }

    ret = key_value_fds_value_read_pos( NULL, file_name_start, data, *len, file_name_offset );
    if( ret >= 0 )
    {
        // *len = ret;
        return 0;    
    }
    hal_mcu_trace_print("key_value_fds_value_read_pos error,ret:%02x\r\n",ret);
    return ret;
}

uint8_t write_lfs_file_one( uint16_t file_name, uint8_t *data, uint16_t len )
{
    int ret = 0xff;
    uint16_t file_name_start = 0;
    uint32_t file_name_offset = 0;

    if( file_name >= POS_FILE_ID_START && file_name <= POS_FILE_ID_END )
    {
        // file_name_start = POS_FILE_ID_START;
        // file_name_offset = ( file_name - file_name_start ) * ( len );
        file_name_start = POS_FILE_ID_START + ( file_name - POS_FILE_ID_START ) / POS_SAVE_IN_FILE_MAX;
        file_name_offset = (( file_name - POS_FILE_ID_START ) % POS_SAVE_IN_FILE_MAX ) * ( len );
    }
    else
    {
        return ret;
    }

    ret = key_value_fds_value_write_pos( NULL, file_name_start, data, len, file_name_offset );
    if( ret >= 0 )
    {
        return 0;    
    }
    hal_mcu_trace_print("key_value_fds_value_write_pos error,ret:%02x\r\n",ret);
    return ret;
}


uint8_t delete_lfs_file( uint16_t file_name )
{
    int ret = 0;
    return ret;
}

void param_init_func(void)
{
    uint16_t param_len = 0;
    uint16_t pos_info_len = 0;    
    uint8_t ret=0;
    uint8_t u8_crc=0;
    app_param_t app_param_temp = {0};
    param_len = sizeof(app_param_t);

    //flash - Factory Settings
    lorawan_param_init();

    //flash - Internal File System
    ret = key_value_fds_init();
    if(ret != 0)
    {
        hal_mcu_trace_print("key_value_fds_init:%02x\r\n",ret);
    }    
    ret = read_lfs_file(DEV_INFO_FILE,(uint8_t *)&app_param_temp,&param_len);

    //init current param 
    if(ret != 0)
    {
        //use code default
        app_param.param_len = ((uint8_t *)&app_param.crc-(uint8_t *)&app_param.param_len);
        app_param.crc = crc8((uint8_t *)&app_param, app_param.param_len);
        ret = write_lfs_file(DEV_INFO_FILE, (uint8_t *)&app_param, sizeof(app_param_t));
        if(ret != 0)
        {
            hal_mcu_trace_print("app_param,len:%02x,crc:%02x\r\n",app_param.param_len,app_param.crc);
        }
    }
    else
    {
        //checksum
        u8_crc = crc8((uint8_t *)&app_param_temp, app_param_temp.param_len);
        if(u8_crc != app_param_temp.crc)
        {
            hal_mcu_trace_print("param crc error,u8_crc:%02x,crc:%02x\r\n",u8_crc,app_param_temp.crc);       
            app_param.param_len = ((uint8_t *)&app_param.crc-(uint8_t *)&app_param.param_len);
            app_param.crc = crc8((uint8_t *)&app_param, app_param.param_len);
            ret = write_lfs_file(DEV_INFO_FILE, (uint8_t *)&app_param, sizeof(app_param_t));
            if(ret != 0)
            {
                hal_mcu_trace_print("app_param,len:%02x,crc:%02x\r\n",app_param.param_len,app_param.crc);
            }
        }
        else
        {
            memcpy(&app_param,&app_param_temp,param_len);         
        }   
    }
    //default param
    param_len = sizeof(app_param_t);
    ret = read_lfs_file(DEF_DEV_INFO_FILE,(uint8_t *)&app_param_temp,&param_len);
    if(ret != 0)
    {
        //init default param
        ret = write_lfs_file(DEF_DEV_INFO_FILE, (uint8_t *)&app_param, sizeof(app_param_t));
        if(ret != 0)
        {
            hal_mcu_trace_print("default app_param,len:%02x,crc:%02x\r\n",app_param.param_len,app_param.crc); 
        }         
    }
    else
    {
        //checksum
        u8_crc = crc8((uint8_t *)&app_param_temp, app_param_temp.param_len);
        if(u8_crc != app_param_temp.crc)
        {
            hal_mcu_trace_print("param crc error,u8_crc:%02x,crc:%02x\r\n",u8_crc,app_param_temp.crc);       
            app_param.param_len = ((uint8_t *)&app_param.crc-(uint8_t *)&app_param.param_len);
            app_param.crc = crc8((uint8_t *)&app_param, app_param.param_len);
            ret = write_lfs_file(DEF_DEV_INFO_FILE, (uint8_t *)&app_param, sizeof(app_param_t));
            if(ret != 0)
            {
                hal_mcu_trace_print("app_param,len:%02x,crc:%02x\r\n",app_param.param_len,app_param.crc);
            }
        }
    }

    //pos msg info
    pos_info_len = sizeof(pos_msg_file_id_t);
    ret = read_lfs_file(POS_INFO_FILE,(uint8_t *)&pos_msg_file_id,&pos_info_len);
    if( ret != 0 || pos_msg_file_id.log_cnt > POS_SAVE_COUNT_MAX || pos_msg_file_id.file_id_start > POS_FILE_ID_END || pos_msg_file_id.file_id_cur > POS_FILE_ID_END )
    {
        //init default log file id
        pos_msg_file_id.reboot_cnt = 0;
        pos_msg_file_id.log_cnt = 0;
        pos_msg_file_id.file_id_start = POS_FILE_ID_START;        
        pos_msg_file_id.file_id_cur = POS_FILE_ID_START;
        ret = write_lfs_file(POS_INFO_FILE, (uint8_t *)&pos_msg_file_id, sizeof(pos_msg_file_id_t));
        if(ret != 0)
        {
            hal_mcu_trace_print("default pos msg info,code:%02x\r\n",ret); 
        }          
    }
    else
    {
        if(pos_msg_file_id.file_id_start < POS_FILE_ID_START ||pos_msg_file_id.file_id_cur<POS_FILE_ID_START)
        {
            pos_msg_file_id.file_id_start = POS_FILE_ID_START;
            pos_msg_file_id.file_id_cur = POS_FILE_ID_START;
            pos_msg_file_id.reboot_cnt = 0;
            pos_msg_file_id.log_cnt = 0;
        }    
        pos_msg_file_id.reboot_cnt++;   
        ret = write_lfs_file(POS_INFO_FILE, (uint8_t *)&pos_msg_file_id, sizeof(pos_msg_file_id_t));
        if(ret != 0)
        {
            hal_mcu_trace_print("default pos msg info,code:%02x\r\n",ret); 
        }           
    }
    gnss_group_id_param_init();

    hal_mcu_trace_print("track_gnss_group_id:%d\r\n",track_gnss_group_id); 

    app_append_param_init();
}


bool write_default_param_config(void)
{
    int ret = 0;

    app_param.param_len = ((uint8_t *)&app_param.crc-(uint8_t *)&app_param.param_len);
    app_param.crc = crc8((uint8_t *)&app_param, app_param.param_len);    

    ret = write_lfs_file(DEF_DEV_INFO_FILE,(uint8_t *)&app_param,sizeof(app_param_t));
    if(ret >= 0)
    {
        return true;
    }
    return false;
}
bool read_default_param_config(void) 
{
    uint16_t r_len = sizeof(app_param_t);
    if (read_lfs_file(DEF_DEV_INFO_FILE, (uint8_t *)&app_param, &r_len))
        return false;
    return true;
}

bool default_factory_eui_get(uint8_t *def_eui)
{
    app_param_t app_param_temp; 
    uint16_t r_len = sizeof(app_param_t);
    if (read_lfs_file(DEF_DEV_INFO_FILE, (uint8_t *)&app_param_temp, &r_len))
        return false;
    memcpy(def_eui,app_param_temp.lora_info.DevEui,8);
    return true;

}

bool write_current_param_config(void)
{
    int ret = 0;

    app_param.param_len = ((uint8_t *)&app_param.crc-(uint8_t *)&app_param.param_len);
    app_param.crc = crc8((uint8_t *)&app_param, app_param.param_len);    

    ret = write_lfs_file(DEV_INFO_FILE,(uint8_t *)&app_param,sizeof(app_param_t));
    if(ret >= 0)
    {
        return true;
    }
    return false;
}

bool read_current_param_config(void) 
{
    uint16_t r_len = sizeof(app_param_t);
    if (read_lfs_file(DEV_INFO_FILE, (uint8_t *)&app_param, &r_len))
        return false;
    return true;
}


bool restore_lorawan_param_config(void) 
{
    app_param_t app_param_temp;
    uint16_t r_len = sizeof(app_param_t);

    if (read_lfs_file(DEF_DEV_INFO_FILE, (uint8_t *)&app_param_temp, &r_len))
        return false;
    r_len = sizeof(lora_info_t);
    memcpy((uint8_t *)&app_param.lora_info,(uint8_t *)&app_param_temp.lora_info,r_len);
    return true;
}

bool reset_factory_param(void)
{
    if (!read_default_param_config()) 
    {
        return false;
    } 
    else 
    {        
        if (!save_Config()) 
        {
            return false;
        }
        return true;
    }
}


uint8_t write_position_msg(void)
{
    uint8_t rc = 0;
    uint16_t app_tag_id= 0;
    uint16_t pos_data_len= 0;  

    //on the basis of  cur file id to save log data
    app_tag_id = pos_msg_file_id.file_id_cur;

    //data integration

    //update log data
    pos_data_len = sizeof(pos_msg_param_t);
    rc = write_lfs_file_one(app_tag_id, (uint8_t *)&pos_msg_param, pos_data_len);
    if (rc != 0)
    {
        hal_mcu_trace_print("write position msg error,code:%d\r\n",rc);
        return rc;
    }
    rc = reflash_pos_msg_info();
    if (rc != 0)
    {         
        hal_mcu_trace_print("reflash log_info error,code:%d\r\n",rc);
        return rc;
    }    
    return rc;  
}


uint8_t reflash_pos_msg_info(void)
{
    uint8_t rc = 0;
    uint16_t rlen = 0;

    pos_msg_file_id.file_id_cur++;
    pos_msg_file_id.log_cnt++;

//    PRINTF("file_id_start:%04x,file_id_cur:%04x log_cnt:%d\r\n",pos_msg_file_id.file_id_start,pos_msg_file_id.file_id_cur,pos_msg_file_id.log_cnt); 
    if(pos_msg_file_id.file_id_cur > POS_FILE_ID_END) //judge file id range
    {
        pos_msg_file_id.file_id_cur = POS_FILE_ID_START;
    }
    if(pos_msg_file_id.log_cnt > POS_SAVE_COUNT_MAX)
    {
        pos_msg_file_id.log_cnt = POS_SAVE_COUNT_MAX;
        pos_msg_file_id.file_id_start++;
        if(pos_msg_file_id.file_id_start > POS_FILE_ID_END) //judge file id range
        {
            pos_msg_file_id.file_id_start = POS_FILE_ID_START;
        }
    }
    rc = write_lfs_file(POS_INFO_FILE, (uint8_t *)&pos_msg_file_id, sizeof(pos_msg_file_id_t));
    if (rc != 0)
    {
        rlen = sizeof(pos_msg_file_id_t);
        read_lfs_file(POS_INFO_FILE,(uint8_t *)&pos_msg_file_id, &rlen);   
        hal_mcu_trace_print("reflash log_info error,code:%d\r\n",rc); 
        return rc;       
    }
    return 0;
}


bool get_pos_msg_info(void)
{
    uint8_t rc = 0;
    uint16_t rlen = 0;
    rlen = sizeof(pos_msg_file_id_t);
    rc = read_lfs_file(POS_INFO_FILE,(uint8_t *)&pos_msg_file_id, &rlen); 
    if (rc != 0)
    {
        return false;
    }
    return true;
}

void get_pos_msg_data(void)
{
    uint8_t rc = 0;
    uint16_t log_real_cnt = 0,log_fail_cnt = 0;
    uint16_t app_tag_id = 0;
    uint16_t rlen = sizeof(pos_msg_param_t);

    if(pos_msg_file_id.log_cnt == 0) return;
    //on the basis of  cur file id to get log data
    if(pos_msg_file_id.file_id_start < pos_msg_file_id.file_id_cur)
    {
        app_tag_id = pos_msg_file_id.file_id_start;
        while(app_tag_id < pos_msg_file_id.file_id_cur)
        {           
            rc = read_lfs_file_one(app_tag_id, (uint8_t*)&pos_msg_param, &rlen); 
            if (rc == 0) 
            {   
                print_pos_msg_data();    
                app_tag_id ++;  
                log_real_cnt ++;      
            }
            else
            {
                app_tag_id ++;  
                log_fail_cnt++;
            }
        }
    }
    else 
    {
        app_tag_id = pos_msg_file_id.file_id_start;
        while(app_tag_id <= POS_FILE_ID_END)
        {             

            rc = read_lfs_file_one(app_tag_id, (uint8_t*)&pos_msg_param, &rlen); 
            if (rc == 0) 
            {
                print_pos_msg_data(); 
                app_tag_id ++;  
                log_real_cnt ++;  
            }
            else
            {
                app_tag_id ++;  
                log_fail_cnt++;
            }
        } 
        app_tag_id = POS_FILE_ID_START;
        while(app_tag_id < pos_msg_file_id.file_id_cur)
        {             
            rc = read_lfs_file_one(app_tag_id, (uint8_t*)&pos_msg_param, &rlen); 
            if (rc == 0) 
            {
                print_pos_msg_data(); 
                app_tag_id ++;  
                log_real_cnt ++;  
            }
            else
            {
                app_tag_id ++;
                log_fail_cnt++;  
            }
        }
    }
    hal_mcu_trace_print("log_real_cnt:%d  log_fail_cnt:%d\r\n",log_real_cnt,log_fail_cnt);                                  
}

uint16_t get_pos_msg_cnt(void)
{
    return pos_msg_file_id.log_cnt;
}



bool read_pos_data(pos_msg_param_t *pos_msg_temp,bool is_old)
{
    uint8_t rc = 0;
    uint16_t app_tag_id = 0;
    uint16_t rlen = sizeof(pos_msg_param_t);

    if(pos_msg_file_id.log_cnt == 0) return false;    

    if(is_old)
    {
        app_tag_id = pos_msg_file_id.file_id_start;
    }
    else
    {
        app_tag_id = pos_msg_file_id.file_id_cur-1;
        if(app_tag_id < POS_FILE_ID_START)
        {
            app_tag_id = POS_FILE_ID_END;
        }
    }
    rc = read_lfs_file_one(app_tag_id, (uint8_t*)pos_msg_temp, &rlen);   
    if (rc == 0)  
    {
        return true;
    }
    return false;
}


bool read_sequence_pos_data(pos_msg_param_t *pos_msg_temp,bool is_old,uint16_t sequence_num)
{
    uint8_t rc = 0;
    uint16_t app_tag_id = 0;
    uint16_t rlen = sizeof(pos_msg_param_t);

    if(pos_msg_file_id.log_cnt == 0) return false;    

    if(sequence_num == 0 || sequence_num > pos_msg_file_id.log_cnt) return false;   

    if(is_old)
    {
        if(pos_msg_file_id.file_id_start+sequence_num-1 > POS_FILE_ID_END)
        {
            app_tag_id = pos_msg_file_id.file_id_start+sequence_num-1 - POS_FILE_ID_END + POS_FILE_ID_START-1;
        }
        else
        {
            app_tag_id = pos_msg_file_id.file_id_start+sequence_num-1;
        }         
    }
    else
    {
        if(pos_msg_file_id.file_id_cur-sequence_num < POS_FILE_ID_START)
        {
            app_tag_id = POS_FILE_ID_END-(POS_FILE_ID_START-(pos_msg_file_id.file_id_cur-sequence_num))+1;
        }
        else
        {
            app_tag_id = pos_msg_file_id.file_id_cur-sequence_num;
        }

    }
    rc = read_lfs_file_one(app_tag_id, (uint8_t*)pos_msg_temp, &rlen);   
    if (rc == 0)  
    {
        return true;
    }
    return false;
}


uint8_t delete_position_msg(void)
{
    return delete_pos_msg_datas(delete_datas_cnt,delete_datas_fifo);
}

uint8_t delete_pos_msg_datas(uint16_t del_cnt,bool is_old)
{
    uint8_t rc = 0;
    uint16_t app_tag_id = 0;
    uint16_t cursor_dir = 0;    
    uint8_t del_status = 0;
    uint16_t rlen1 = sizeof(pos_msg_file_id_t);
    pos_msg_file_id_t pos_msg_file_temp;
    memset(&pos_msg_file_temp,0,rlen1);
    
    
    if(pos_msg_file_id.log_cnt < del_cnt) return 1;    

    hal_mcu_trace_print("delete pos msg count:%d,%d\r\n",del_cnt,pos_msg_file_id.log_cnt); 

    hal_mcu_trace_print("start:%04x,end:%04x,log_cnt:%d\r\n",pos_msg_file_id.file_id_start,pos_msg_file_id.file_id_cur,pos_msg_file_id.log_cnt); 
    
    if(is_old)
    {
        app_tag_id = pos_msg_file_id.file_id_start;
        cursor_dir = 1;
    }
    else
    {
        app_tag_id = pos_msg_file_id.file_id_cur-1;
        cursor_dir = 0;       
    }
    for(uint16_t u16i = 0; u16i < del_cnt; u16i++)
    {
        del_status = 0;
        rc = delete_lfs_file(app_tag_id); 
        if (rc == 0)  
        {
            pos_msg_file_id.log_cnt--;
            if(cursor_dir == 1)
            {            
                pos_msg_file_id.file_id_start++;
                if(pos_msg_file_id.file_id_start > POS_FILE_ID_END) //judge file id range
                {
                    pos_msg_file_id.file_id_start = POS_FILE_ID_START;
                }
            }
            else
            {
                if(pos_msg_file_id.file_id_cur < POS_FILE_ID_START) //judge file id range
                {
                    pos_msg_file_id.file_id_cur = POS_FILE_ID_END;
                }                
            }             
            rc = write_lfs_file(POS_INFO_FILE, (uint8_t *)&pos_msg_file_id, sizeof(pos_msg_file_id_t));
            if (rc != 0)
            {
                rlen1 = sizeof(pos_msg_file_id_t);
                read_lfs_file(POS_INFO_FILE,(uint8_t *)&pos_msg_file_id, &rlen1);   
                hal_mcu_trace_print("delete_pos_datas but reflash log_info error,code:%d\r\n",rc); 
                return rc;       
            }
            hal_mcu_trace_print("u_start:%04x,u_end:%04x,u_log_cnt:%d\r\n",pos_msg_file_id.file_id_start,pos_msg_file_id.file_id_cur,pos_msg_file_id.log_cnt); 
        }
        else if(rc != 1)
        {
            hal_mcu_trace_print("delete_pos_datas error,code:%d\r\n",rc);             
            return rc;            
        }
        else
        {
            del_status = 1;
        }
        if(cursor_dir == 1)
        {
            app_tag_id = pos_msg_file_id.file_id_start;
            if(del_status == 1)
            {
                pos_msg_file_id.log_cnt--;
                app_tag_id = pos_msg_file_id.file_id_start++;
                if(pos_msg_file_id.file_id_start > POS_FILE_ID_END) //judge file id range
                {
                    pos_msg_file_id.file_id_start = POS_FILE_ID_START;
                }
            }
        }
        else
        {
            app_tag_id = app_tag_id-1;   
            if(app_tag_id < POS_FILE_ID_START)
            {
                app_tag_id = POS_FILE_ID_END;
            }
            if(del_status == 1)
            {
                
            }
        }      
    }
    rlen1 = sizeof(pos_msg_file_id_t);
    read_lfs_file(POS_INFO_FILE,(uint8_t *)&pos_msg_file_temp, &rlen1); 
    hal_mcu_trace_print("start:%04x,end:%04x,log_cnt:%d\r\n",pos_msg_file_id.file_id_start,pos_msg_file_id.file_id_cur,pos_msg_file_id.log_cnt); 
    hal_mcu_trace_print("t_start:%04x,t_end:%04x,t_log_cnt:%d\r\n",pos_msg_file_temp.file_id_start,pos_msg_file_temp.file_id_cur,pos_msg_file_temp.log_cnt); 

    return 0;
}

void print_pos_msg_data(void)
{
    if( pos_msg_param.pos_type == pos_state )
    {
        hal_mcu_trace_print( "utc:%u,pos_state:%d,event_status:%d\r\n",pos_msg_param.utc_time, ( pos_msg_param.pos_status >> 24 ) & 0xff,pos_msg_param.pos_status & 0xffffff );
    }
    else
    {
        hal_mcu_trace_print( "utc:%u,event_status:%d,motion_index:%d,",pos_msg_param.utc_time, ( pos_msg_param.pos_status >> 8 ) & 0xffffff, pos_msg_param.pos_status & 0xff );
        switch(pos_msg_param.pos_type)
        {
            case  pos_gnss_raw:
                    hal_mcu_trace_print("gnss: zone_flag:%u, group_id:%u, len:%u\r\n", pos_msg_param.context.gps_context.zone_flag,\
                                                                            pos_msg_param.context.gps_context.group_id,\
                                                                            pos_msg_param.context.gps_context.gnss_len);  
                    for(uint8_t u8i = 0;u8i<pos_msg_param.context.gps_context.gnss_len;u8i++)
                    {
                        hal_mcu_trace_print("%02x ",pos_msg_param.context.gps_context.gnss_res[u8i]);  
                    }
                    hal_mcu_trace_print("\r\n");  
            case  pos_wifi:
                    hal_mcu_trace_print("WIFI: \r\n");      
                    for(uint8_t u8i = 0;u8i<pos_msg_param.context_count;u8i++)
                    {
                        hal_mcu_trace_print("mac:%02x:%02x:%02x:%02x:%02x:%02x,rssi:%d\r\n", pos_msg_param.context.wifi_context[u8i].wifi_mac[5],\
                                                                    pos_msg_param.context.wifi_context[u8i].wifi_mac[4],\
                                                                    pos_msg_param.context.wifi_context[u8i].wifi_mac[3],\
                                                                    pos_msg_param.context.wifi_context[u8i].wifi_mac[2],\
                                                                    pos_msg_param.context.wifi_context[u8i].wifi_mac[1],\
                                                                    pos_msg_param.context.wifi_context[u8i].wifi_mac[0],\
                                                                    pos_msg_param.context.wifi_context[u8i].cur_rssi); // 
                    }
                break; 
            case  pos_beac:
                    hal_mcu_trace_print("BEACON: \r\n");     
                    for(uint8_t u8i = 0;u8i<pos_msg_param.context_count;u8i++)
                    {
                        hal_mcu_trace_print("mac:%02x,%02x,%02x,%02x,%02x,%02x,rssi:%d\r\n",pos_msg_param.context.beac_context[u8i].beac_mac[5],\
                                                                    pos_msg_param.context.beac_context[u8i].beac_mac[4],\
                                                                    pos_msg_param.context.beac_context[u8i].beac_mac[3],\
                                                                    pos_msg_param.context.beac_context[u8i].beac_mac[2],\
                                                                    pos_msg_param.context.beac_context[u8i].beac_mac[1],\
                                                                    pos_msg_param.context.beac_context[u8i].beac_mac[0],\
                                                                    pos_msg_param.context.beac_context[u8i].cur_rssi); // 
                    }                
                    break;                                
            default:   
                    break;  
        }
    }
}


/***************** gnss group id ************************/
bool read_gnss_group_id_param(void) 
{
    uint8_t ret = 0;
    uint16_t test_data_len= 0;  
    test_data_len = sizeof(gnss_group_param_t);
    ret = read_lfs_file(GROUP_ID_INFO_FILE,(uint8_t *)&app_gnss_group_param,&test_data_len);
    if(ret == 0)
    {
        track_gnss_group_id = app_gnss_group_param.group_id;
        return true;        
    }    
    return false;
}

uint8_t write_gnss_group_id_param(void)
{
    uint8_t rc = 0;
    uint16_t test_data_len= 0;  
    //
    app_gnss_group_param.group_id = track_gnss_group_id;
    test_data_len = sizeof(gnss_group_param_t);
    rc = write_lfs_file(GROUP_ID_INFO_FILE, (uint8_t *)&app_gnss_group_param, test_data_len);
    if (rc != 0)
    {
        hal_mcu_trace_print("write_gnss_group_id_param error,code:%d\r\n",rc);
        return rc;
    } 
    return 0;  
}

void gnss_group_id_param_init(void) 
{
    uint8_t ret = 0;    
    uint16_t test_data_len= 0;  
    uint32_t group_id_temp;
    uint8_t len = sizeof(group_id_temp);

    lfs_t *lfs;
    lfs_file_t file;
    lfs = InternalFS._getFS();
    ret = lfs_file_open(lfs, &file, FILENAME, LFS_O_RDONLY);
    // file existed
    if ( ret == 0 )
    {
        ret = lfs_file_rewind(lfs, &file);
        len = lfs_file_read(lfs, &file, &group_id_temp, len);
        lfs_file_close(lfs, &file);
        track_gnss_group_id = group_id_temp;
        app_gnss_group_param.group_id =  track_gnss_group_id;
        //Live Storage Migration
        ret = write_lfs_file(GROUP_ID_INFO_FILE, (uint8_t *)&app_gnss_group_param, sizeof(gnss_group_param_t));
        if(ret != 0)
        {
            hal_mcu_trace_print("gnss_group_id_param_init error,code:%02x\r\n",ret); 
        }          
        else
        {
            lfs_remove(lfs, FILENAME);
        }
    }
    else
    {
        //
        test_data_len = sizeof(gnss_group_param_t );
        ret = read_lfs_file(GROUP_ID_INFO_FILE,(uint8_t *)&app_gnss_group_param,&test_data_len);
        if(ret != 0)
        {
            ret = write_lfs_file(GROUP_ID_INFO_FILE, (uint8_t *)&app_gnss_group_param, sizeof(gnss_group_param_t));
            if(ret != 0)
            {
                hal_mcu_trace_print("gnss_group_id_param_init error,code:%02x\r\n",ret); 
            }  
            track_gnss_group_id = app_gnss_group_param.group_id;            
        } 
        else{
            track_gnss_group_id = app_gnss_group_param.group_id;  
        }  
    }   
}


/***************** app append parameter ************************/
bool read_app_append_param(void) 
{
    uint8_t ret = 0;
    uint16_t test_data_len= 0;  
    test_data_len = sizeof(append_param_t);
    ret = read_lfs_file(APPEND_PARAM_INFO_FILE,(uint8_t *)&app_append_param,&test_data_len);
    if(ret == 0)
    {
        return true;        
    }    
    return false;
}

uint8_t write_app_append_param(void)
{
    uint8_t rc = 0;
    uint16_t test_data_len= 0;  
    //
    test_data_len = sizeof(append_param_t);
    rc = write_lfs_file(APPEND_PARAM_INFO_FILE, (uint8_t *)&app_append_param, test_data_len);
    if (rc != 0)
    {
        hal_mcu_trace_print("write_app_append_param error,code:%d\r\n",rc);
        return rc;
    } 
    return 0;  
}

void app_append_param_init(void) 
{
    uint8_t ret = 0;    
    uint16_t test_data_len= 0;  

    test_data_len = sizeof(append_param_t );
    ret = read_lfs_file(APPEND_PARAM_INFO_FILE,(uint8_t *)&app_append_param,&test_data_len);
    if(ret != 0)
    {
        ret = write_lfs_file(APPEND_PARAM_INFO_FILE, (uint8_t *)&app_append_param, sizeof(append_param_t));
        if(ret != 0)
        {
            hal_mcu_trace_print("app_append_param_init error,code:%02x\r\n",ret); 
        }         
    }   
}
