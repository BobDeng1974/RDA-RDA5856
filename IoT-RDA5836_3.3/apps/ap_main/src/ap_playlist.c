/*
 * Copyright (c) 2006-2016 RDA Microelectronics, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "ap_common.h"

#ifdef MULTI_DIR_TRAVERSAL_SUPPORT
UINT16 fselPlaylistItemCur;
UINT16 fselPlaylistItemTotal;


/********************************************************************************
* Description : init the play list
*
* Arguments  :
*
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :
*
********************************************************************************/
BOOL fselPlayListInit()
{
    INT32 fd = -1;
    UINT64 ino = 0;
    UINT64 fstino = 0;
    UINT8 buf[11] = {0};
    UINT16 count = 0;
    fselPlaylistItemCur = 0;
    fselPlaylistItemTotal = 0;
    FS_ChangeDir(FS_CD_ROOT);
    fd = FS_Open((PCSTR)PLAYLIST_PATH, FS_O_RDWR | FS_O_TRUNC, 0);
    if(ERR_FS_NO_DIR_ENTRY == fd)
	{
		fd = FS_Create((PCSTR)PLAYLIST_PATH, 0);
	}

	if(fd < 0)
		return FALSE;

    while(SelectNextDisk(&ino))
    {
        if(count == 0)
            fstino = ino;

        if(fstino == ino && count > 0)
            break;

        count++;
        memcpy(&buf[0], &count, 2);
        memcpy(&buf[2], &ino, 8);
        buf[10] = '\n';
        FS_Write(fd, buf, 11);
    }

    fselPlaylistItemTotal = count;
    fselPlaylistItemCur = 0;
    FS_Close(fd);
    app_trace(APP_MUSIC_TRC, "Init Playlist success!");
    return TRUE;
}

#endif

#ifdef APP_SUPPORT_PLAYLIST
HANDLE   fpPlaylist;
LISTITEM * plistitem;
WCHAR    PlayListFileName[12];
WCHAR    ItemFileName[12];

UINT16    PlaylistClassTotal;
UINT16    PlaylistItemTotal;
UINT16    PlaylistCurClass;
UINT16    PlaylistCurItem;

UINT32    ClassStrOffset;
UINT16    ClassStrLen;
UINT16    ClassItemTotal;
UINT32    Class1stItemOffset;
UINT8    PlaylistTotal;;//�õ��ܲ����б���?
INT8    listtype[2];
UINT16    fileoffset;

//global variable should be resident
extern UINT8 fselError;  // store error id;


INT8    fSelBuf[512];    //read file buffer



static void DmaCpy1(UINT16 descadd, UINT8 descpg, UINT16 srcadd, UINT8 srcpg, UINT16 Length);


/********************************************************************************
* Description : enter the play list mode system
*
* Arguments  :
*             filename : the playlist file name
              retInfo: classname
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :
*
********************************************************************************/
BOOL fselPlayListEnt(WCHAR *filename)    //filename: ARTIST.A3L, ALBUM.A3L, ....
{
    WCHAR *lName[]={L"PLAYLIST"};

    memcpy(PlayListFileName, filename, 12);  //keep the play list file name

    FS_ChangeDir(FS_CD_ROOT);    //return the root

    if ( ERR_SUCCESS!=FS_ChangeDir(lName)) //enter the playlist
    {
        fselError = FSEL_ERR_FS;
        return FALSE;
    }

    if ( fpPlaylist )
    {
        FS_Close(fpPlaylist);
    }

    fpPlaylist = FS_Open(filename, FS_O_RDONLY,0);  //open the play list filename

    if ( !fpPlaylist )
    {
        fselError = FSEL_ERR_FS;
        return FALSE;
    }

    if ( (listtype[0] != filename[0]) || (listtype[1] != filename[1]) ) //�Ƚ�ǰ�����ַ����㹻�ֱ治ͬ��list�ļ�
    {
        PlaylistCurClass = 0;               //��ʼ��
        PlaylistCurItem  = 0;                //��ʼ��
        listtype[0] = filename[0];
        listtype[1] = filename[1];
    }

    fileoffset = 0;

    if ( ERR_SUCCESS==FS_Seek(fpPlaylist,fileoffset,FS_SEEK_SET))  //get the class parament
    {

        if ( ERR_SUCCESS==FS_Read(fpPlaylist,fSelBuf,512) )
        {
            PlaylistClassTotal = * ((UINT16 *)&fSelBuf[0x20]);        //ȡ��ClassTotal

            if ( PlaylistClassTotal == 0 )
            {
                fselError = FSEL_ERR_PLAYLIST;
                FS_ChangeDir(FS_CD_ROOT);
                return FALSE;
            }
            else
            {
                PlaylistItemTotal = * ((UINT16 *)&fSelBuf[0x22]);     //ȡ��ItemTotal
                FS_ChangeDir(FS_CD_ROOT);
                return TRUE;
            }
        }
        else    //Read err
        {
            FS_ChangeDir(FS_CD_ROOT);
            return FALSE;
        }
    }
    else        //Seek err
    {
        FS_ChangeDir(FS_CD_ROOT);
        return FALSE;
    }
}

/********************************************************************************
* Description : get the class parament
*
* Arguments  :
*             classname : output the next classname
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :it is called by playlistclassPrev and playlistclassNext
*
********************************************************************************/
BOOL getClassParam(WCHAR *classname, UINT16 strlen)
{
    WCHAR *str;
    CATMEMB * pcatmemb;

    if ( (PlaylistCurClass == 0) || (PlaylistCurClass > PlaylistClassTotal) )
    {
        PlaylistCurClass = 1;
    }

    fileoffset = (PlaylistCurClass-1)/32 + 1;         //��ǰclass��¼��list�ļ��ڼ�������
    pcatmemb = (CATMEMB * )(fSelBuf + ((PlaylistCurClass-1) % 32) * 16);  //��ǰclass��¼��������ַ

    if (ERR_SUCCESS== FS_Seek(fpPlaylist,fileoffset,FS_SEEK_SET))      //seek
    {

        if( ERR_SUCCESS==FS_Read(fpPlaylist,fSelBuf,512) )      //read
        {
            ClassStrOffset = pcatmemb->stroffset;
            ClassStrLen = pcatmemb->strlength;
            ClassItemTotal = pcatmemb->MembItemNum;
            Class1stItemOffset = pcatmemb->FistItemOffset;

            if ( classname == NULL )    //ֻȡ��Ϣ����ȡclass����
            {
                return TRUE;
            }
            else
            {
                fileoffset = ClassStrOffset / 0x200;         //��ǰclass��¼��list�ļ��ڼ�������
                FS_Seek(fpPlaylist,fileoffset,FS_SEEK_SET);
				FS_Read(fpPlaylist,fSelBuf,512);

                DmaCpy1( FILEDATABUF, 0x06, (UINT16)&fSelBuf, 0x08, 0x200-1 );

                str = (INT8*)FILEDATABUF + ClassStrOffset % 0x200;
                if ( (str+ClassStrLen) > ((INT8*)FILEDATABUF+0x200) )   //strԽ����
                {
					FS_Read(fpPlaylist,fSelBuf,512);
                    DmaCpy1( FILEDATABUF+0x200, 0x06, (UINT16)&fSelBuf, 0x08, 0x200-1 );
                }

                if ( (strlen == -1) || (strlen > ClassStrLen) )
                {
                    INT8 * p;

                    memcpy(classname, str, ClassStrLen);        //class name <--------
                    classname[ClassStrLen-2] = 0x20;        //�ӿո����ڹ���
                    classname[ClassStrLen-1] = 0x0;          //�ӿո����ڹ���
                    p = (INT8 *)&classname[ClassStrLen-1];
                    p++;  * p = 0x0;  p++;  * p = 0x0;      //��һ��unicode������
                }
                else if ( strlen <= ClassStrLen )
                {
                    memcpy(classname, str, strlen);
                    classname[strlen-4] = 0x20;        //�ӿո����ڹ���
                    classname[strlen-3] = 0x0;          //�ӿո����ڹ���
                    classname[strlen-2] = 0x0;          //�ӿո����ڹ���
                    classname[strlen-1] = 0x0;          //�ӿո����ڹ���
                }
                return TRUE;
            }
        }
        else
            return FALSE;
    }
    else
        return FALSE;
}




static void DmaCpy1(UINT16 descadd, UINT8 descpg, UINT16 srcadd, UINT8 srcpg, UINT16 Length)
{

}

/********************************************************************************
* Description : read the sector of current item in list file
*
* Arguments  : sector buffer

* Returns    :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :
*
********************************************************************************/
static BOOL ReadCurItemSector(INT8 * Buffer)    //ͬһbank,�����static,ʹ�䲻��bank call
{
    UINT16 itemfileoffset;
    long itemoffset;

    itemoffset = Class1stItemOffset + (PlaylistCurItem-1)*sizeof(LISTITEM);
    itemfileoffset = itemoffset / 0x200;        //��ǰitem��¼��list�ļ��ڼ�������

	plistitem = (LISTITEM *)(Buffer + itemoffset % 0x200);

    fileoffset = itemfileoffset;

    if ( ERR_SUCCESS==FS_Seek(fpPlaylist,fileoffset,FS_SEEK_SET) )  //seek
    {
        if( ERR_SUCCESS==FS_Read(fpPlaylist,Buffer,512))  //read
        {
            return TRUE;
        }
        else
        {
            fselError = FSEL_ERR_FS;
            return FALSE;
        }
    }
    else
    {
        fselError = FSEL_ERR_FS;
        return FALSE;
    }

}

/********************************************************************************
* Description : get the play list filename
*
* Arguments  :
*             filename : the selector filename

* Returns     :*
*               0: Fail, list�ļ�����
                1: Successful,
                2��//����������
* Notes       :
*
********************************************************************************/
UINT8 getplayItemparam(WCHAR *filename)
{
    if ( ReadCurItemSector(fSelBuf) )   //Ĭ�����룺PlaylistCurItem
    {
        if ( plistitem->FirstCluster == 0xffffffff || plistitem->FileEntry  == 0xffffffff )
        {
            return 2;   //����������
        }

        if ( FS_SetCurDirEntry(plistitem->FirstCluster) )        //��λĿ¼
        {
            if ( FS_SetCurDirEntry(plistitem->FileEntry) )  //��λ�ļ�
            {
                if ( ERR_SUCCESS==FS_GetEntryName(plistitem->FileEntry, MAX_FILE_LEN, filename) )
                {                                       //���ֱ�Ӵӵ�ǰĿ¼����Ҹ�Ŀ¼�����������NameBuf�ĵ�һ���ֽڳ�ʼ��Ϊ20h����,CharNumΪ0���ʾ�ӵ�ǰĿ¼��ֱ�ӻ�ȡ�ļ�8+3���͵Ķ���
                    memcpy(ItemFileName, filename, MAX_FILE_LEN);  //�����ļ���
                    return 1;
                }
                else
                {
                    return 2;   //����������
                }
            }
            else
            {
                return 2;   //����������
            }
        }
        else
        {
            return 2;       //����������
        }
    }
    else
    {
        return 0;   //list�ļ�����
    }
}

/********************************************************************************
* Description : get the next playlist class
*
* Arguments  :
*             classname : output the next classname
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :it is called by fselGetNextGroup.
*
********************************************************************************/
BOOL playlistclassNext(WCHAR *classname)
{
    if ( PlaylistClassTotal == 0 )    //total class = 0
    {
        return FALSE;
    }

    PlaylistCurItem = 0;    //��ʼ��

    if ( PlaylistCurClass < PlaylistClassTotal )  //0 ~ PlaylistClassTotal
    {
        PlaylistCurClass++;  //next no
    }
    else
    {
        return FALSE;       //�ѵ����һ��class
    }

    return ( getClassParam(classname,LISTNAME_DISP_LENNGTH) );  //Ĭ�����룺PlaylistCurClass
}

/********************************************************************************
* Description : get the Prev playlist class
*
* Arguments  :
*             classname : output the next classname
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :it is called by fselGetPrevGroup.
*
********************************************************************************/
BOOL playlistclassPrev(WCHAR *classname)
{
    if ( PlaylistClassTotal == 0 )    //classtotal = 0
    {
        return FALSE;
    }

    PlaylistCurItem = 0;    //��ʼ��

    if ( PlaylistCurClass > 1 )             // if the prev class is the first, get the first again
    {
        PlaylistCurClass--;
    }
    else
    {
        return FALSE;           //�ѵ���ǰһ��class
    }

    return ( getClassParam(classname,LISTNAME_DISP_LENNGTH) );    //Ĭ�����룺PlaylistCurClass
}

/********************************************************************************
* Description : get the next song in curretn class
*
* Arguments  :
*             filename : the selector filename

* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :
*
********************************************************************************/
BOOL playlistnext(WCHAR *filename)
{
    UINT8 iRet;

RETRYNEXT:
    if ( PlaylistCurItem < ClassItemTotal )     //1 ~ ClassItemTotal
    {
        PlaylistCurItem++;
    }

    iRet = getplayItemparam(filename);  //Ĭ�����룺PlaylistCurItem

    if ( iRet == 2 )                    //play list index has removed
    {
        if ( PlaylistCurItem < ClassItemTotal )
        {
            goto RETRYNEXT;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return iRet;
    }
}


/********************************************************************************
* Description : get the prev song in curretn class
*
* Arguments  :
*             filename : the selector filename

* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :
*
********************************************************************************/
BOOL playlistprev(WCHAR *filename)
{
    UINT8 iRet;

RETRYPREV:
    if ( PlaylistCurItem > 1 )     //1 ~ ClassItemTotal
    {
        PlaylistCurItem--;
    }

    iRet = getplayItemparam(filename);  //Ĭ�����룺PlaylistCurItem

    if ( iRet == 2 )                    //play list index has removed
    {
        if ( PlaylistCurItem > 1 )
        {
            goto RETRYPREV;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return iRet;
    }
}


/********************************************************************************
* Description : del the current file in play list
*
* Arguments  :
*             filename : return the current selector file name
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :it is called by fselPlayDel
*
********************************************************************************/
BOOL playlistdel(WCHAR *filename)
{
    if ( getplayItemparam( filename ) == 1 )     //ȡ��Ҫɾ���ļ����ļ���
    {
        if ( ReadCurItemSector(fSelBuf) )           //Ĭ�����룺PlaylistCurItem
        {
            plistitem->FirstCluster = 0xffffffff;       //list�ļ���item����Ϊ��Ч
            plistitem->FileEntry    = 0xffffffff;

            if ( ERR_SUCCESS==FS_Write(fpPlaylist,fSelBuf,sizeof(fSelBuf)))  //!!!!!!!!!!!!!!!!!sdk90�ļ�ϵͳĿǰ��֧���޸��ļ�!!!!!!!!!!!!!!!!!!!!!!!!
            {
                return TRUE;
            }
            else
            {
                fselError = FSEL_ERR_FS;
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
}


/********************************************************************************
* Description : close the palylist file and return to common file system
*
* Arguments  :
*
* Returns     :
*            TRUE: Successful,
*            FALSE: Fail, detail information in fselError.
* Notes       :
*
********************************************************************************/
BOOL fselPlayListExit(void)
{
    fselError = 0;
    if ( FS_Close(fpPlaylist)<0 )
        return FALSE;
    else
        return TRUE;
}

#endif



