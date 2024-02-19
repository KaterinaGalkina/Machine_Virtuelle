#define REGISTRE_MAX 32767
#define REGISTRE_MIN -32768
extern unsigned char Mem[65536];
extern short int R[32];
extern int CodeOp;
extern int dest;
extern int source1;
extern int imm;
extern int source2;
extern int Z, C, N;
extern unsigned char *PC;
extern int nombreDeLignes;

int hexaADecimal(char ch);
int int216bit(int x);
void VIDE(void);
void ADD(void);
void SUB(void);
void MUL(void);
void DIV(void);
void AND(void);
void OR(void);
void XOR(void);
void SHL(void);
void LDB(void);
void LDW(void);
void STB(void);
void STW(void);
void JMP(void);
void JZS(void);
void JZC(void);
void JCS(void);
void JCC(void);
void JNS(void);
void JNC(void);
void IN(void);
void OUT(void);
void RND(void);
void HLT(void);
void programInMem(int *nombreDeLignes);
void ligne(unsigned char *line, int *CodeOp, int *dest, int *source1, int *imm, int *source2);

extern void (*tabCommandes[32])(void);