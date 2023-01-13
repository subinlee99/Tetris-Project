#include "tetris.h"
#include <unistd.h>

static struct sigaction act, oact;

void init_root(){
    recRoot = NULL;
    recRoot = malloc(sizeof(RecNode));
    (recRoot->lv)=0;
    (recRoot->score) = score;
    for(int i=0; i<HEIGHT;i++){
        for(int j=0; j<WIDTH;j++){
            recRoot->field[i][j] = field[i][j];
        }
    }

    
}
int main(){
	int exit=0;
	initscr();
	noecho();
    keypad(stdscr, TRUE);
	srand((unsigned int)time(NULL));
    createRankList();
	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
        case MENU_RANK: rank(); break;
        case MENU_REC: recommendedPlay(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}
    
    writeRankFile();
	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
    nextBlock[2]=rand()%7;
    init_root();
    recommend(recRoot);
    
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
    if(rec_flag==1){
        blockRotate=recommendR;
        blockX=recommendX;

    }
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
    
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
    DrawBox(9,WIDTH+10,4,8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);  //A_REVERSE:Reverse video
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
        move(10+i,WIDTH+13);
        for( j = 0; j < 4; j++ ){
            if( block[nextBlock[2]][0][i][j] == 1 ){
                attron(A_REVERSE);  //A_REVERSE:Reverse video
                printw(" ");
                attroff(A_REVERSE);
            }
        else printw(" ");
	}
}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();

	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
            alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();
			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/*TETRIS WEEK 1*/

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
    int i, j;
    for(i=0;i<4;i++){
        for(j=0;j<4;j++){
            if(block[currentBlock][blockRotate][i][j]==1){
                if((j+blockX)>=0 && (j+blockX)<WIDTH && (i+blockY)<HEIGHT && (i+blockY)>=0){ if(f[i+blockY][j+blockX]==1) return 0; //블럭이 필드 범위 안에 있지만 이미 블럭이 있음
                }
               else return 0; //블럭이 범위 밖에 있음
            }
        }
    }
    return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
    int del_y=0; int del_x=0; int pre_Rotate=blockRotate;
	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
    //2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
    switch(command){
        case KEY_RIGHT:
            del_x=-1;
            break;
        case KEY_LEFT:
            del_x=1;
            break;
        case KEY_DOWN:
            del_y=-1;
            break;
        case KEY_UP:
            pre_Rotate = (blockRotate+3)%4;
            break;
    }
    char tile = '.';

    int shadowY=blockY+1;
    while(CheckToMove(f, currentBlock, pre_Rotate,++shadowY, blockX+del_x)==1);
    for(int i=0; i<4;i++){
        for(int j=0;j<4;j++){
            if(block[currentBlock][pre_Rotate][i][j]==1){
                move(i+blockY+1+del_y,j+blockX+1+del_x);
                //attorn(A_REVERSE);
                printw("%c", tile);
                
                move(i+shadowY+del_y,j+blockX+1+del_x);
                printw("%c", tile);
                //attorn(A_REVERSE);
                
            }
        }
    }
	//3. 새로운 블록 정보를 그린다.
    //DrawBlock(blockY, blockX, nextBlock[0],blockRotate, ' ');
    DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);

}

void BlockDown(int sig){
    if(rec_flag == 1) {
        blockX = recommendX;
        blockRotate = recommendR;
    }
    if(CheckToMove(field,nextBlock[0],blockRotate, blockY+1, blockX)==1){ //내려갈 수 있으면
        blockY+=1;
        DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
        move(HEIGHT,WIDTH);
        timed_out=0; //block 매 초마다 한 칸씩 떨어트림
        return;
    }
  
    //더 이상 내려가지 못하면
    if(blockY==-1) gameOver=1; //gameover
    score+=AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);
    score+=DeleteLine(field);
   
    PrintScore(score);
    init_block();
    init_root();//reset block
    recommend(recRoot);
    
    DrawNextBlock(nextBlock);
    DrawField();
    timed_out=0;
    return;
    
	//강의자료 p26-27의 플로우차트를 참고한다.
}
void init_block(){ //update block - same as init
    //updata next block
    nextBlock[0]=nextBlock[1];
    nextBlock[1]=nextBlock[2];
    nextBlock[2]=rand()%7;
    blockRotate=0; blockY=-1; blockX=WIDTH/2-2;
    
    
    
    timed_out=0;//block 매 초마다 한 칸씩 떨어트림
}
int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
    int touched=0;
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++)
            if(block[currentBlock][blockRotate][i][j]==1){
                f[blockY+i][blockX+j]=1;
                if(f[blockY+i+1][blockX+j]==1 || (blockY+i+1)==HEIGHT){
                    touched++;
                }
            }
    }
    return  touched*10;
	//Block이 추가된 영역의 필드값을 바꾼다.
}

int DeleteLine(char f[HEIGHT][WIDTH]){ //다시하기
	// user code
    int N=0; //number of line to delete
    int flag=0; //1 if line is full
    for(int i=0;i<HEIGHT;i++){
        //1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
        int j=0; flag=1;
        while(j<WIDTH){
            if(f[i][j++]!=1){
                flag=0;
                break;
            }
        }
        //2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
        if(flag==1){
            N++;
            for(j=i; j>0;j--)
                for(int k=0;k<WIDTH;k++)f[j][k]=f[j-1][k];
        }
    }

    return N*N*100;
}

/*TETRIS WEEK 2 - SHADOW*/

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
    
    while(CheckToMove(field, blockID, blockRotate,++y, x)==1);
    DrawBlock(y-1, x, blockID, blockRotate, '/');
    
}
void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
    DrawShadow(y, x, blockID, blockRotate);
    DrawRecommend(recommendY, recommendX, blockID, recommendR);
    DrawBlock(y, x, blockID, blockRotate, ' ');
    
}

/* TETRIS WEEK 3
 Linked list to save ranking*/

Node* newNode(int new_data, char *new_name)
{
   
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->name = (char*)malloc(sizeof(char) * (strlen(new_name)+1));
    strcpy(new_node->name, new_name);
    new_node->data = new_data;
    new_node->next = NULL;

    return new_node;
}

void sortedInsert(Node** h, Node* new_node){
    Node* curr = *h;
    if (*h == NULL || (*h)->data <= new_node->data) {
        new_node->next = *h;
        *h = new_node;
    }
    else {
        while (curr->next && curr->next->data > new_node->data) {
            curr = curr->next;
        }
        new_node->next = curr->next;
        curr->next = new_node;
    }
 
}

void printRank(Node** head, int start, int end)
{
    int count =1;
    Node* tmp = *head;
    while (tmp && count <=end) {
        if(count>=start && count<=end) printw("%20s | %15d\n", tmp->name, tmp->data);
        tmp = tmp->next;
        count++;
    }
}
int find_name(Node **head, char *search_name){
    Node *tmp = *head;
    int flag=0;
    while(tmp){
        if(*tmp->name == *search_name){
            printw("%20s | %15d\n", tmp->name, tmp->data);
            flag=1;
        }
        tmp=tmp->next;
    }
    return flag;
}
bool find_rank(Node **head, int rank){
    Node *cur = *head;
    Node *prev = *head;
    int count =1;
    bool flag=false;
    while(cur){
        if(count == rank){
            prev->next=cur->next;
            flag=true;
            break;
        }
        prev=cur;
        cur=cur->next;
        count++;
    }
    return flag;
}


/* TETRIS WEEK3
 ranking system*/
void createRankList(){
    FILE* fp;
    char name[NAMELEN+1]; int score; Node* new;
    fp = fopen("rank.txt", "r");
    if(!fp) printw("file cannot opens");
    else{
        fscanf(fp, "%d", &score_number);
        for(int i =0 ; i < score_number; i++) {
            fscanf(fp, "%s %d", name, &score);
            new = newNode(score,name);
            sortedInsert(&head, new);
        }
    }
    fclose(fp);
}

void rank(){
    
    //목적: rank 메뉴를 출력하고 점수 순으로 X부터~Y까지 출력함
    //1. 문자열 초기화
    int X=1; int Y= score_number; int ch=0;
    clear();
    //2. printw()로 3개의 메뉴출력
    printw("1. list ranks from X to Y\n");
    printw("2. list ranks by a specific name\n");
    printw("3. delete a specific rank\n");
    int x_init =1; int y_init=5;
    //3. wgetch()를 사용하여 변수 ch에 입력받은 메뉴번호 저장
    ch = wgetch(stdscr);
    //4. 각 메뉴에 따라 입력받을 값을 변수에 저장
    //4-1. 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의 rank 출력
    if (ch == '1') {
        echo();
        printw("X: "); scanw("%d",&X);
        printw("Y: "); scanw("%d",&Y);
        noecho();
        printw("     name            |         score      \n");
        printw("------------------------------------------\n");
        if(X>Y) {
            printw("\nsearch failure: no rank in the list \n");
          
        }
        else{
            if(X<1 || X>score_number) X=x_init;
            if(Y<1 || Y>score_number) Y=y_init;
            printRank(&head, X,  Y);
        }
    }
        else if (ch=='2'){ //list rank by name
            char name[NAMELEN+1];
            echo();
            printw("input the name: "); scanw("%c",name);
            noecho();
            printw("     name            |         score      \n");
            printw("------------------------------------------\n");
     
            if(!find_name(&head, name)) printw("\nsearch failure: no name in the list");
    }
        else if (ch=='3'){ //delete specific rank
            int rank;
            echo();
            printw("input the rank: "); scanw("%d",&rank);
            noecho();

            if(find_rank(&head, rank))printw("\nresult: the rank deleted");
            else printw("\nsearch failure: the rank not in the list");
        }
     
    getch();
}

void writeRankFile(){

    FILE *fp = fopen("rank.txt", "w");
    
    if(!fp) printw("file cannot open\n");
    
    fprintf(fp, "%d\n", score_number);
    
    Node* tmp = head;
    for(int i=0;i<score_number;i++){
        fprintf(fp ,"%s %d\n", tmp->name, tmp->data);
        tmp = tmp->next;
        if(!tmp) break;
    }
    fclose(fp);
}

void newRank(int score){
 
    char str[NAMELEN+1];score_number++;
    clear();
    echo();
    printw("your name ");
    scanw("%s",str);
    noecho();

    Node* new_node = newNode(score, str);
    sortedInsert(&head, new_node);
 
    writeRankFile();
     
}
/*TETRIS WEEK 4 - RECOMMEND */

void DrawRecommend(int y, int x, int blockID,int blockRotate){
    DrawBlock(y, x, blockID, blockRotate, 'R');
}

RecNode* newChild(RecNode *parent){
    RecNode *child = (RecNode*)(sizeof(RecNode));
    child->lv = ((parent->lv)+1);
    child->score=(parent->score);
    for(int i=0; i<HEIGHT;i++){
        for(int j=0; j<WIDTH;j++){
            child->field[i][j] = parent->field[i][j];
        }
    }
    return child;
}

int recommend(RecNode* root){
    int max_score=0;
    int tempY, tempX;
    for(int r = 0; r < 4; r++) {
        //for each position possible to put the block
        for(tempX = -1; tempX < WIDTH-1; tempX++) {
            
            RecNode* child;
            child = malloc(sizeof(RecNode));
            child->lv = ((root->lv)+1);
            child->score=(root->score);
            for(int i=0; i<HEIGHT;i++){
                for(int j=0; j<WIDTH;j++){
                    child->field[i][j] = root->field[i][j];
                }
            }
             
            //RecNode* child;
            //child= newChild(root);
            tempY = 0;
            while(CheckToMove(field, nextBlock[(child->lv)-1], r,++tempY, tempX)==1);
            tempY--;
            child->score += AddBlockToField(child->field, nextBlock[(child->lv)-1], r, tempY,tempX);
            child->score += (DeleteLine(child->field));
            if(child->lv < MAX_LEVEL) {
                child->score += recommend(child);
            }
            if(max_score < child->score) {
                if(root->lv == 0) {
                    recommendR = r;
                    recommendY = tempY;recommendX = tempX;
                    max_score = child->score;
                }
                else max_score = child->score;
            }
        }
    }
            
    return max_score;
}


void recommendedPlay(){ //자동화된 play 모드
    rec_flag = 1;
    play();
    rec_flag = 0;
}
