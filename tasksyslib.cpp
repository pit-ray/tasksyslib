#include "tasksyslib.h"

#include <stdlib.h>
#include <string.h>
TaskSystem *InitTaskSys( unsigned int max_task_num ) {
    TaskSystem *p_task_sys = ( TaskSystem * ) malloc( TASK_SYS_SIZE ) ;
    memset( p_task_sys, 0, TASK_SYS_SIZE ) ;

    unsigned int alloc_size   = ( unsigned int ) ( TASK_SIZE * max_task_num ) ;

    p_task_sys->p_mem_head = ( unsigned char * ) malloc( alloc_size ) ;

    //メモリ領域の割り当てに成功したかどうか確認する
    if( p_task_sys->p_mem_head == NULL ) {
        return NULL ;
    }

    //タスクシステム構造体領域の初期化
    p_task_sys->max_task_num = max_task_num ;
    p_task_sys->alloc_size   = alloc_size ;
    p_task_sys->p_first_task     = NULL ;
    p_task_sys->p_last_task      = NULL ;
    p_task_sys->all_task_size    = 0 ;
    p_task_sys->all_task_num     = 0 ;
    p_task_sys->active_task_size = 0 ;
    p_task_sys->active_task_num  = 0 ;

    //タスクシステム領域のゼロクリア
    memset( p_task_sys->p_mem_head, 0, p_task_sys->alloc_size ) ;

    return p_task_sys ;
}


//タスクの中身を設定する
void SetTaskParameter(
    Task         *p_task,
    unsigned int priority,
    void         ( *p_FuncAccessed ) ( Task * ),
    void         *p_resource
) {

    p_task->priority       = priority ;
    p_task->active_flag    = false ;
    p_task->p_resource     = p_resource ;
    p_task->p_FuncAccessed = p_FuncAccessed ;
    p_task->p_next_task    = NULL ;
    p_task->p_pre_task     = NULL ;

    return ;

}

//タスクシステムにタスクを登録する
//戻り値:登録したタスクの先頭アドレス
Task *RegisterNewTask( TaskSystem *p_task_sys ) {

//エラー処理
    if( p_task_sys->p_mem_head == NULL ) {
        return NULL ;
    }

    //タスクを追加して最大タスク数を超すかどうか確かめる
    if( p_task_sys->all_task_num == p_task_sys->max_task_num ) {
        return NULL ;
    }

//

    p_task_sys->all_task_size += TASK_SIZE ;
    p_task_sys->all_task_num ++ ;

    //タスクシステムを先頭から走査
    for( unsigned int num_i = 1 ;
         num_i <= p_task_sys->max_task_num ;
         num_i ++ ) {

        //タスク単位ごとに走査する
        Task *p_task = ( Task * ) ( p_task_sys->p_mem_head + TASK_SIZE * ( num_i - 1 ) ) ;

        //空きタスクならばそこに追加する
        if( p_task->register_flag == false ) {
            p_task->register_flag = true ;
            return p_task ;
        }
    }

    return NULL ;
}

//タスクシステムに登録済みのタスクをアクティブにする
bool IsActivateRegdTask( TaskSystem *p_task_sys, Task *p_regd_task ) {

//エラー処理
    if( p_task_sys->p_mem_head == NULL ) {
        return false ;
    }
    if( p_regd_task == NULL ) {
        return false ;
    }
    //重複防止
    if( p_regd_task->active_flag == true ) {
        return false ;
    }
//

    p_regd_task->active_flag = true ;

    p_task_sys->active_task_size += TASK_SIZE ;
    p_task_sys->active_task_num ++ ;

    //他にアクティブタスクが存在しないとき
    if( p_task_sys->active_task_num == 1 ) {
        p_regd_task->p_pre_task  = NULL ;
        p_regd_task->p_next_task = NULL ;

        p_task_sys->p_first_task = p_regd_task ;
        p_task_sys->p_last_task  = p_regd_task ;

        return true ;
    }

    //以下二つのif文は処理高速化のため、追加位置が最初か最後かをとりわけて比較している
    if( p_regd_task->priority >= p_task_sys->p_first_task->priority ) {
        p_regd_task->p_pre_task              = NULL ;
        p_regd_task->p_next_task             = p_task_sys->p_first_task ;

        p_task_sys->p_first_task->p_pre_task = p_regd_task ;
        p_task_sys->p_first_task             = p_regd_task ;

        return true ;
    }
    if( p_regd_task->priority <= p_task_sys->p_last_task->priority ) {
        p_regd_task->p_pre_task              = p_task_sys->p_last_task ;
        p_regd_task->p_next_task             = NULL ;

        p_task_sys->p_last_task->p_next_task = p_regd_task ;
        p_task_sys->p_last_task              = p_regd_task ;

        return true ;
    }

    Task *task_i = p_task_sys->p_first_task->p_next_task ;
    while( 1 ) {
        //追加するタスク以下の優先度をもつタスクの前に追加する
        if( p_regd_task->priority >= task_i->priority ) {
            task_i->p_pre_task->p_next_task = p_regd_task ;

            p_regd_task->p_pre_task  = task_i->p_pre_task ;
            p_regd_task->p_next_task = task_i ;

            task_i->p_pre_task = p_regd_task ;

            break ;
        }
        task_i = task_i->p_next_task ;
    }
    return true ;
}

//タスクシステムに登録済みのタスクを非アクティブにする
bool IsDeactivateRegdTask( TaskSystem *p_task_sys, Task *p_regd_task ) {

//エラー処理
    if( p_task_sys->p_mem_head == NULL ) {
        return false ;
    }
    if( p_regd_task == NULL ) {
        return false ;
    }
    //重複防止
    if( p_regd_task->active_flag == false ) {
        return false ;
    }
//

    p_regd_task->active_flag = false ;

    p_task_sys->active_task_size -= TASK_SIZE ;
    p_task_sys->active_task_num -- ;

    //最後のアクティブタスクのとき
    if( p_task_sys->active_task_num == 0 ) {
        p_task_sys->p_first_task = NULL ;
        p_task_sys->p_last_task  = NULL ;

        return true ;
    }

    //削除するタスクが最初に実行するタスクのとき
    if( p_regd_task == p_task_sys->p_first_task ) {
        p_regd_task->p_next_task->p_pre_task = NULL ;
        p_task_sys->p_first_task             = p_regd_task->p_next_task ;
        p_regd_task->p_next_task               = NULL ;

        return true ;
    }

    //削除するタスクが最後に実行するタスクのとき
    if( p_regd_task == p_task_sys->p_last_task ) {
        p_regd_task->p_pre_task->p_next_task = NULL ;
        p_task_sys->p_last_task              = p_regd_task->p_pre_task ;
        p_regd_task->p_pre_task              = NULL ;

        return true ;
    }

    p_regd_task->p_pre_task->p_next_task = p_regd_task->p_next_task ;
    p_regd_task->p_next_task->p_pre_task = p_regd_task->p_pre_task ;

    p_regd_task->p_pre_task  = NULL ;
    p_regd_task->p_next_task = NULL ;

    return true ;
}

//タスクシステム上からタスクを完全削除する。
//IsDeactivateTaskの機能もカバーしている
bool IsDelTask( TaskSystem *p_task_sys, Task *p_regd_task ) {

//エラー
    if( p_task_sys->p_mem_head == NULL ) {
        return false ;
    }
    if( p_regd_task == NULL ) {
        return false ;
    }
//

    //必ず非アクティブ様態にする
    if( p_regd_task->active_flag == true ) {
        if( !IsDeactivateRegdTask( p_task_sys, p_regd_task ) ) {
            return false ;
        }
    }

    p_task_sys->all_task_size -= TASK_SIZE ;
    p_task_sys->all_task_num -- ;

    memset( p_regd_task, 0, TASK_SIZE ) ;
    p_regd_task = NULL ;

    return true ;

}

//タスクシステム上から非アクティブタスクをすべて完全削除する。
bool IsDelAllInactiveTask( TaskSystem *p_task_sys ) {

//エラー処理
    if( p_task_sys->p_mem_head == NULL ) {
        return false ;
    }
//

    //非アクティブタスクなし
    if( GetInactiveTaskNum( p_task_sys ) == 0 ) {
        return true ;
    }

    for( unsigned int num_i = 1 ;
         num_i <= p_task_sys->max_task_num ;
         num_i ++ ) {

        Task *task_i = ( Task * ) ( p_task_sys->p_mem_head + TASK_SIZE * ( num_i - 1 ) ) ;

        if( task_i->register_flag == true ) {
            if( task_i->active_flag != true ) {
                memset( task_i, 0, TASK_SIZE ) ;
                task_i = NULL ;

                p_task_sys->all_task_size -= TASK_SIZE ;
                p_task_sys->all_task_num -- ;
            }
        }
    }
    return true ;
}

//アクティブタスクの処理関数を優先度にしたがって呼び出す
bool IsCallActiveFunc( TaskSystem *p_task_sys ) {

//エラー処理
    if( p_task_sys->p_mem_head == NULL ) {
        return false ;
    }
//
    if( p_task_sys->active_task_num == 1 ) {
        p_task_sys->p_first_task->p_FuncAccessed( p_task_sys->p_first_task ) ;
        return true ;
    }

    //アクティブタスクが二つ以上
    Task* task_i = p_task_sys->p_first_task ;
    while( task_i != NULL ) {

         task_i->p_FuncAccessed( task_i ) ;

         if( task_i->p_next_task == NULL ) {
             break;
         }
         task_i = task_i->p_next_task ;
    }

    return true ;
}

//タスクシステムの後始末をする。
bool IsDeinitTaskSys( TaskSystem *p_task_sys ) {

//エラー処理
    if( p_task_sys->p_mem_head == NULL ) {
        return false ;
    }
//

    free( p_task_sys->p_mem_head ) ;
    p_task_sys->p_mem_head = NULL ;

    p_task_sys->p_first_task = NULL ;
    p_task_sys->p_last_task  = NULL ;
    return true ;
}