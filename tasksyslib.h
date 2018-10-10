/*
Library   : tasksyslib
License   : MIT
GitHub    : https://github.com/pit-ray/tasksyslib

Developer : pit-ray
Twitter   : @pitxray
Blog      : https://www.pit-ray.com
E-mail    : pitxray@gmail.com

_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
*/
#ifndef TASKSYSLIB_H
#define TASKSYSLIB_H

//Cで作った関数や変数はC++用に変換する
extern "C" {
    /*変数とマクロ*/

    typedef struct TaskParameter {

        bool                 register_flag ;                                  //タスクがタスクシステム上に存在するかというフラグ
        unsigned int         priority ;                                       //タスクの優先順位
        bool                 active_flag ;                                    //タスクの状態([true]アクティブ,[flase]非アクティブ)

        void                 *p_resource ;                                    //タスクが使うデータへのポインタ
        void                 ( *p_FuncAccessed ) ( struct TaskParameter * ) ; //タスクが処理する関数へのポインタ

        struct TaskParameter *p_next_task ;                                   //このタスクの次に処理するタスクへのポインタ
        struct TaskParameter *p_pre_task ;                                    //このタスクの前に処理するタスクへのポインタ

    } Task ;

    typedef struct TaskSystemParameter {

        unsigned char *p_mem_head ;       //確保したメモリ領域の先頭アドレスへのポインタ
        unsigned int  alloc_size ;        //確保するメモリのサイズ
        unsigned int  max_task_num ;      //最大タスク数

        Task          *p_first_task ;     //最初に実行するタスクへのポインタ
        Task          *p_last_task ;      //最後に実行するタスクへのポインタ

        unsigned int  all_task_size ;     //タスクシステムに登録されている全タスクの合計サイズ
        unsigned int  all_task_num ;      //タスクシステムに登録されている全タスクの合計数

        unsigned int  active_task_size ;  //タスクリストのサイズ
        unsigned int  active_task_num ;   //タスクリストに登録されたタスクの合計
    } TaskSystem ;


    #define TASK_SIZE     sizeof( Task )
    #define TASK_SYS_SIZE sizeof( TaskSystem )


    /*関数*/

    //タスクシステムを初期化する
    TaskSystem *InitTaskSys(
        unsigned int max_task_num
    ) ;

    //タスクの中身を設定する
    void SetTaskParameter(
        Task         *p_task,
        unsigned int priority,
        void         ( *p_FuncAccessed ) ( Task * ),
        void         *p_resource
    ) ;

    //タスクシステムにタスクを登録する
    //戻り値:登録したタスクの先頭アドレス
    Task *RegisterNewTask(
        TaskSystem *p_task_sys
    ) ;

    //タスクシステムに登録済みのタスクをアクティブにする
    bool IsActivateRegdTask(
        TaskSystem *p_task_sys,
        Task       *p_regd_task
    ) ;

    //タスクシステムに登録済みのタスクを非アクティブにする
    bool IsDeactivateRegdTask(
        TaskSystem *p_task_sys,
        Task       *p_regd_task
    ) ;

    //タスクシステム上からタスクを完全削除する。
    //IsDeactivateTaskの機能もカバーしている
    bool IsDelTask(
        TaskSystem *p_task_sys,
        Task       *p_regd_task
    ) ;

    //タスクシステム上から非アクティブタスクをすべて完全削除する。
    bool IsDelAllInactiveTask(
        TaskSystem *p_task_sys
    ) ;

    //アクティブタスクの処理関数を優先度にしたがって呼び出す
    bool IsCallActiveFunc(
        TaskSystem *p_task_sys
    ) ;

    //タスクシステムの後始末をする。
    bool IsDeinitTaskSys(
        TaskSystem *p_task_sys
    ) ;

    /*インライン関数*/

    //全てのタスクの合計サイズを取得する（バイト単位)
    inline unsigned int GetAllTaskSize( TaskSystem *p_task_sys ) {
        return p_task_sys->all_task_size ;
    }

    //全てのタスクの合計数を取得する
    inline unsigned int GetAllTaskNum( TaskSystem *p_task_sys ) {
        return p_task_sys->all_task_num ;
    }

    //アクティブタスクの合計サイズを取得する（バイト単位)
    inline unsigned int GetActiveTaskSize( TaskSystem *p_task_sys ) {
        return p_task_sys->active_task_size ;
    }

    //アクティブタスクの合計数を取得する
    inline unsigned int GetActiveTaskNum( TaskSystem *p_task_sys ) {
        return p_task_sys->active_task_num ;
    }

    //非アクティブタスクの合計サイズを取得する（バイト単位)
    inline unsigned int GetInactiveTaskSize( TaskSystem *p_task_sys ) {
        return ( p_task_sys->all_task_size - p_task_sys->active_task_size ) ;
    }

    //非アクティブタスクの合計数を取得する
    inline unsigned int GetInactiveTaskNum( TaskSystem *p_task_sys ) {
        return ( p_task_sys->all_task_num - p_task_sys->active_task_num ) ;
    }
}
#endif
