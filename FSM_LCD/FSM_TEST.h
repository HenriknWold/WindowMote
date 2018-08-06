#define EJECTED 0
#define INSERTING 1
#define INSERTED 2
#define EJECTING 3

typedef struct tSystem
{
	int TFWD;
	int TREV;
	int SFWD;
	int SREV;
	int current_state;
	int next_state;
	int nTimeFwdLeft;
	int nTimeRevLeft;
	int n;
} TSystem;
