
#ifndef __LIBT2FS___
#define __LIBT2FS___

#define TYPEVAL_REGULAR     0x01
#define TYPEVAL_DIRETORIO   0x02
#define TYPEVAL_INVALIDO    0xFF

#define SECTOR_SIZE		256

typedef int FILE2;
typedef int DIR2;

typedef unsigned char BYTE;
typedef unsigned short int WORD;
typedef unsigned int DWORD;

/** Superbloco */
struct t2fs_superbloco {
    char    Id[4];          /* Identifica��o do sistema de arquivo. � formado pelas letras �T2FS�. */
    WORD    Version;        /* Vers�o atual desse sistema de arquivos: (valor fixo 7DE=2014; 2=2 semestre). */
    WORD    SuperBlockSize; /* Quantidade de setores  que formam o superbloco. (fixo em 1 setor) */
    DWORD   DiskSize;       /* Tamanho total da parti��o T2FS, incluindo o tamanho do superblock. */
    DWORD   NofBlocks;      /* Quantidade total de blocos de dados na parti��o T2FS (1024 blocos). */
    DWORD   BlockSize;      /* Tamanho de um bloco.*/
    DWORD   BitmapBlocks;	/* Primeiro bloco l�gico do bitmap de blocos livres e ocupados*/
    DWORD   BitmapInodes;	/* Primeiro bloco l�gico do bitmap de i-nodes livres e ocupados.*/
    DWORD   InodeBlock;		/* Primeiro bloco l�gico da �rea de i-nodes*/
    DWORD   FirstDataBlock;	/* Primeiro bloco l�gico da �rea de blocos de dados.*/
} __attribute__((packed));

/** Registro de diret�rio (entrada de diret�rio) */
struct t2fs_record {
    BYTE    TypeVal;        /* Tipo da entrada. Indica se o registro � inv�lido (0xFF), arquivo (0x01 ou diret�rio (0x02) */
    char    name[31];       /* Nome do arquivo. : string com caracteres ASCII (0x21 at� 0x7A), case sensitive. */
    DWORD   blocksFileSize; /* Tamanho do arquivo, expresso em n�mero de blocos de dados */
    DWORD   bytesFileSize;  /* Tamanho do arquivo. Expresso em n�mero de bytes. */
    DWORD   i_node;         /* i-node do arquivo */
    char    Reserved[20];
} __attribute__((packed));

/** i-node */
struct t2fs_inode {
    DWORD   dataPtr[10];    /* Ponteiros diretos para blocos de dados do arquivo */
    DWORD   singleIndPtr;   /* Ponteiro de indire��o simples */
    DWORD   doubleIndPtr;   /* Ponteiro de indire��o dupla */
    char    Reserved[16];   /* N�o usados */
} __attribute__((packed));

#define MAX_FILE_NAME_SIZE 255
typedef struct {
    char name[MAX_FILE_NAME_SIZE+1];
    int fileType;   // ==1, is directory; ==0 is file
    unsigned long fileSize;
} DIRENT2;

/** global variables */
#define NULL_BLOCK 0x0FFFFFFFF
struct t2fs_superbloco _super_block_;
int _inode_size_;
int _record_size_;
int _sectors_per_block_;
int _inodes_per_block_;
int _records_per_block_;
int _current_dir_inode_;

/** internal functions */
int init();
int read_super_block();
int block_to_sector(DWORD block);
int read_block(BYTE* data, DWORD block);
int inode_block(DWORD inode);
int inode_offset(DWORD inode);
int read_inode(struct t2fs_inode *inode_data, DWORD inode);
int record_data_ptr(DWORD record);
int record_offset(DWORD record);
int read_record(struct t2fs_record *dirent_data, struct t2fs_inode *inode, DWORD record);
int find_record(struct t2fs_record *record_data, struct t2fs_inode *inode, char *record_name);
int find_record_data_ptr(struct t2fs_record *record_data, DWORD *data_ptr, int data_size, char *record_name);

/** user functions */
int identify2 (char *name, int size);

FILE2 create2 (char *filename);
int delete2 (char *filename);
FILE2 open2 (char *filename);
int close2 (FILE2 handle);
int read2 (FILE2 handle, char *buffer, int size);
int write2 (FILE2 handle, char *buffer, int size);
int seek2 (FILE2 handle, unsigned int offset);

int mkdir2 (char *pathname);
int rmdir2 (char *pathname);

DIR2 opendir2 (char *pathname);
int readdir2 (DIR2 handle, DIRENT2 *dentry);
int closedir2 (DIR2 handle);

int chdir2 (char *pathname);
int getcwd2 (char *pathname, int size);

#endif
