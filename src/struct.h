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
// ���������� ��������� ��������� (����������� � �������)
/////////////////////////////////////////////////////////////////////////////

// ���� ��������� ���������
typedef struct
{	int bAlwaysOnTop;		// ���� "��������� �� �������"
	int bEasyMove;			// ���� ����������� �� ����� ����� ����
	int nXcoord;			// ��������� �-���������� ����
	int nYcoord;			// ��������� �-���������� ����
	int nDialogIndex;		// ������ ��������� �������� �������� �������
	CString	strLastFilePath;// �������� ���� � ���������� ��������� �����
	CString strLastFileName;// ��� ���������� ��������� ����� (��� ����)
	int nGraphType;			// ������� ������ �����������
	int bGraphMaxpeaks;		// shows maximum peaks
	int bMonitoring;		// ���� ����������� ������
	//int bMInstances;		// ����������� ������� ���������� ����� ���������
	int nPlayVolume;
	int nLoopbackDevice;
} CONF_PROG;

// ��������� �������
typedef struct
{	int		nRecSrcLineID;
	int		nRecSrcLineLevel;
	int		nPlaySrcLineID;
	int		nPlaySrcLineLevel;
	CString strMixShortName;
} CONF_MIXER;

// ��������� ������� "General"
typedef struct
{	int nLoader;	// �������� � ������� ��� �������� ���������
	int bAutoRun;	// ����������� ������ � ������
	int bMinimized;	// ����������� ���������
	int bToolTips;	// ���� ������ ���������
	int bTrayIcon;	// ��������� ������ � ����
	int bTrayMin;	// �������������� � ����
	int bMInstances;// ����������� ������� ���������� ����� ���������
} CONF_DIAL_GEN;

// ��������� ������� "Record"
typedef struct
{	int nBitrate;	// �������� mp3 ��������
	int nFreq;		// �������� �������
	int nStereo;	// ������ ������ ?
	int bAutoFreq;	// ������������� ������� � �������
} CONF_DIAL_MP3;

/*// ��������� ������� "Scheduler"
typedef struct
{	int bEnable;	// Scheduler ������� ?
	int nStartH;	// ��� �������
	int nStartM;	// ������ �������
	int nStartS;	// ������� �������
	int nStopH;		// ��� ��������
	int nStopM;		// ������ ��������
	int nStopS;		// ������� ��������
} CONF_DIAL_SH;*/

// ��������� ������� "Scheduler 2"
typedef struct
{	int h;	// ���
	int m;	// ���.
	int s;	// ���.
} SHR_TIME;

typedef struct
{	int shutDown;
	int closeAll;
} SHR_ACTION;

typedef struct
{	int bIsEnabled;		// ������� �������
	SHR_TIME t_start;	// ����� ��������� ������
	SHR_TIME t_stop;	// ����� ����. ������
	SHR_TIME t_rec;		// ����� ����� ������
	int bSchedStart;	// ���� ���������������� ������ ������
	int bRunExternal;	// ���� ������� ��� ������ �����
	CString strFileName;// ��� ������������ �����
	int nStopByID;		// ��� ������� ���������� (���. ���, ��� �������. �����)
	SHR_ACTION	action;	// �������� ��� ��������
} CONF_DIAL_SH2;

// ��������� ������� "VAS"
typedef struct
{	int bEnable;	// VAS ������� ?
	int nThreshold;	// ��������� �������
	int nWaitTime;	// ����� �������� ������������
	int nAction;	// �������� �� ������������ VAS
} CONF_DIAL_VAS;

// ��������� ������� "File Names"
typedef struct
{	CString strAutoName;	// ������ ����� �����
	CString strTDefault;	// ������ �� ���������
} CONF_DIAL_AN;

/////////////////////////////////////////////////////////////////////////////
// ��������� ������/���������������
/////////////////////////////////////////////////////////////////////////////

// ��������� �������� ��������� ������/���������������
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
// ������ � �������
/////////////////////////////////////////////////////////////////////////////

// ���������������� � �����
#define SND_FILE_BEGIN	 0
#define SND_FILE_CURRENT 1
#define SND_FILE_END	 2

// ��������� ����� �����
#define IN_SECONDS	0
#define IN_KBYTES	1

/////////////////////////////////////////////////////////////////////////////
#endif