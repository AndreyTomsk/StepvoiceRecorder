/////////////////////////////////////////////////////////////////////////////
#ifndef _STRUCT_H_
#define _STRUCT_H_

/********************************************************************/
// ERRORS
/********************************************************************/

// windows WAV recording errors
#define MOD_REC					0
#define MOD_REC_ERR_DEVICE		1
#define MOD_REC_ERR_OPEN		2
#define MOD_REC_ERR_BUFFER		3

// windows WAV playing errors
#define MOD_PLAY				0
#define MOD_PLAY_ERR_DEVICE		1
#define MOD_PLAY_ERR_OPEN		2
#define MOD_PLAY_ERR_BUFFER		3

// encoder errors
#define MOD_ENC_MP3				0
#define MOD_ENC_MP3_ERR_DLL		1
#define MOD_ENC_MP3_ERR_INIT	2

// decoder errors
#define MOD_DEC_MP3				0
#define MOD_DEC_MP3_ERR_DLL		1
#define MOD_DEC_MP3_ERR_INIT	2

// file errors
#define MOD_FILE				0
#define MOD_FILE_ERR_CREATE		1
#define MOD_FILE_ERR_OPEN		2
#define MOD_FILE_ERR_REWRITE	3
#define MOD_FILE_ERR_READ		4
#define MOD_FILE_ERR_WRITE		5


/////////////////////////////////////////////////////////////////////////////
// ГЛОБАЛЬНЫЕ НАСТРОЙКИ ПРОГРАММЫ (СОХРАНЯЮТСЯ В РЕЕСТРЕ)
/////////////////////////////////////////////////////////////////////////////

// Ощие настройки программы
typedef struct
{	int bAlwaysOnTop;		// Флаг "Постоянно на вершине"
	int bEasyMove;			// Флаг перемещения за любую точку окна
	int nXcoord;			// Последняя х-координата окна
	int nYcoord;			// Последняя у-координата окна
	int nDialogIndex;		// Индекс последней открытой страницы диалога
	CString	strLastFilePath;// Значение пути к последнему открытому файлу
	CString strLastFileName;// Имя последнего открытого файла (без пути)
	int nGraphType;			// Текущий график отображения
	int bGraphMaxpeaks;		// shows maximum peaks
	int bMonitoring;		// флаг мониторинга записи
	//int bMInstances;		// возможность запуска нескольких копий программы
	int nPlayVolume;
	int nLoopbackDevice;
} CONF_PROG;

// Настройки микшера
typedef struct
{	int		nRecSrcLineID;
	int		nRecSrcLineLevel;
	int		nPlaySrcLineID;
	int		nPlaySrcLineLevel;
	CString strMixShortName;
} CONF_MIXER;

// Настройки диалога "General"
typedef struct
{	int nLoader;	// Действия с файлами при загрузке программы
	int bAutoRun;	// Запускаться вместе с виндой
	int bMinimized;	// Запускаться свернутым
	int bToolTips;	// Флаг вывода подсказок
	int bTrayIcon;	// Создавать иконку в трее
	int bTrayMin;	// Минимизировать в трей
	int bMInstances;// возможность запуска нескольких копий программы
} CONF_DIAL_GEN;

// Настройки диалога "Record"
typedef struct
{	int nBitrate;	// Значение mp3 битрейта
	int nFreq;		// Значение частоты
	int nStereo;	// Делать стерео ?
	int bAutoFreq;	// Автоустановка частоты в диалоге
} CONF_DIAL_MP3;

/*// Настройки диалога "Scheduler"
typedef struct
{	int bEnable;	// Scheduler включен ?
	int nStartH;	// Час запуска
	int nStartM;	// Минута запуска
	int nStartS;	// Секунда запуска
	int nStopH;		// Час останова
	int nStopM;		// Минута останова
	int nStopS;		// Секунда останова
} CONF_DIAL_SH;*/

// Настройки диалога "Scheduler 2"
typedef struct
{	int h;	// час
	int m;	// мин.
	int s;	// сек.
} SHR_TIME;

typedef struct
{	int shutDown;
	int closeAll;
} SHR_ACTION;

typedef struct
{	int bIsEnabled;		// шедулер включен
	SHR_TIME t_start;	// время включения записи
	SHR_TIME t_stop;	// время выкл. записи
	SHR_TIME t_rec;		// общее время записи
	int bSchedStart;	// флаг запранированного старта записи
	int bRunExternal;	// флаг запуска при старте файла
	CString strFileName;// имя запускаемого файла
	int nStopByID;		// тип времени выключения (общ. зап, или определ. время)
	SHR_ACTION	action;	// действия при останове
} CONF_DIAL_SH2;

// Настройки диалога "VAS"
typedef struct
{	int bEnable;	// VAS включен ?
	int nThreshold;	// Пороговый уровень
	int nWaitTime;	// Время ожидания срабатывания
	int nAction;	// Действие на срабатывание VAS
} CONF_DIAL_VAS;

// Настройки диалога "File Names"
typedef struct
{	CString strAutoName;	// Шаблон имени файла
	CString strTDefault;	// шаблон по умолчанию
} CONF_DIAL_AN;

/////////////////////////////////////////////////////////////////////////////
// ПАРАМЕТРЫ ЗАПИСИ/ВОСПРОИЗВЕДЕНИЯ
/////////////////////////////////////////////////////////////////////////////

// параметры открытия устройств записи/воспроизведения
typedef struct
{	int nBitrate;
	int	nFreq;
	int nBits;
	int nStereo;
} SOUND_INFO;

typedef struct
{	int nFreq;
	int nBits;
	int nChannel;
	int nDevID;
	int nBufNum;
	int nBufSize;
} WAVE_INFO;

typedef struct // !!!! test
{
	int lsf;
	int mpeg25;
	int layer;
	int crc;

	int bitr_index;
	int freq_index;
	int padding;
	int priv_bit;

	int mode;
	int mode_ext;
	int copyright;
	int original;
	int emphasis;

	int framesize;
} MPEG_HEADER;

/////////////////////////////////////////////////////////////////////////////
// РАБОТА С ФАЙЛАМИ
/////////////////////////////////////////////////////////////////////////////

// позиционирование в файле
#define SND_FILE_BEGIN	 0
#define SND_FILE_CURRENT 1
#define SND_FILE_END	 2

// получение длины файла
#define IN_SECONDS	0
#define IN_KBYTES	1

/////////////////////////////////////////////////////////////////////////////
#endif