#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<conio.h>
#include<malloc.h>
#include<string.h>

struct symboltable{
	char name;
	int address;
	int size;
};

struct intermediate{
	int lineno;
	int opcodeno;
	int parameter[6];
};

struct label{
	char lab;
	int seek;
};

char *opcode[20] = { "MOV", "MOV", "ADD", "SUB", "MUL", "DIV", "JMP", "IF", "EQ", "LT", "GT", "LTEQ", "GTEQ", "PRINT", "READ", "ENDIF", "ELSE" };

char *reg[9] = { "AX", "BX", "CX", "DX", "EX", "FX", "GX", "HX" };
int *regvalue = (int *)malloc(sizeof(int) * 8);

int stack[100];
int top = -1;

void push(int *stack, int x){
	top++;
	stack[top] = x;
}
int pop(int *stack){
	int x = stack[top];
	top--;
	return x;
}


void str_cpy(char *s, char *buff){
	int i;
	for (i = 0; buff[i] != '\0'&& buff[i] != '\n'; i++){
		s[i] = buff[i];
	}
	s[i] = '\0';
}


char** parse(char *str)
{
	int j = 0, k = 0;
	char **x = (char**)malloc(sizeof(char) * 10);
	for (int i = 0; i < 10; i++){
		x[i] = (char*)malloc(sizeof(char) * 10);
	}


	for (int i = 0; str[i] != '\0'; i++){

		if (str[i] == ' ' || str[i] == '=' || str[i] == ',' || str[i] == '\n'){
			while (str[i] == ' ' || str[i] == '=' || str[i] == ',' || str[i] == '\n'){
				i++;
			}
			i--;
		}
		else{
			k = 0;
			while (str[i] != ' ' && str[i] != '\0' && str[i] != ',' && str[i] != '='&&str[i] != '\n'){
				x[j][k] = str[i];
				k = k + 1;
				i++;
			}
			x[j][k] = '\0';
			j = j + 1;
			i--;
		}
	}
	return x;
}



void insert_to_symbol(char **x, char *str, int *arr, struct symboltable** s, int *index, int *pos){
	if (strcmp(x[0], "DATA") == 0){
		s[*index]->name = x[1][0];
		s[*index]->address = *pos;

		if (x[1][1] == '[' && x[1][3] == ']'){

			s[*index]->size = x[1][2] - 48;
			*pos = *pos + s[*index]->size;
		}
		else{
			s[*index]->size = 1;
			*pos = *pos + 1;
		}
		*index = *index + 1;
	}

	else if (strcmp(x[0], "CONST") == 0){
		s[*index]->name = x[1][0];
		s[*index]->address = *pos;
		s[*index]->size = 0;

		printf("\n%s\n", x[2]);
		arr[*pos] = x[2][0] - 48;
		*pos = *pos + 1;
		*index = *index + 1;
	}
}

int findRegIndex(char *ch){
	for (int i = 0; i < 8; i++){
		if (strcmp(reg[i], ch) == 0)
			return i;
	}
	return -1;
}

int find_in_table(struct symboltable **s, char *c, int *pos)
{
	printf("%s\n", c);
	for (int i = 0; i < *pos; i++){
		printf("%c\n", s[i]->name);
		if (s[i]->name == c[0])
			return i;
	}
	return -1;
}
void read(int index, char **x, int *intindex, struct intermediate** inter){
	inter[*intindex]->lineno = *intindex;
	inter[*intindex]->opcodeno = index;
	inter[*intindex]->parameter[0] = findRegIndex(x[1]);
	*intindex = *intindex + 1;
}

void add(int index, char **x, int *intindex, struct intermediate** inter){
	inter[*intindex]->lineno = *intindex;
	inter[*intindex]->opcodeno = index;
	//	printf("%s %s %s\n\n", (x[1]), (x[2]), (x[3]));

	//	printf("%d %d %d\n\n", findRegIndex(x[1]), findRegIndex(x[2]), findRegIndex(x[3]));
	inter[*intindex]->parameter[0] = findRegIndex(x[1]);
	inter[*intindex]->parameter[1] = findRegIndex(x[2]);
	inter[*intindex]->parameter[2] = findRegIndex(x[3]);

	*intindex = *intindex + 1;
}

void mov(int index, char **x, int *intindex, struct intermediate** inter, struct symboltable **s, int *pos){
	inter[*intindex]->lineno = *intindex;
	int flag = findRegIndex(x[1]);
	if (flag == -1){
		inter[*intindex]->opcodeno = index;
		inter[*intindex]->parameter[0] = find_in_table(s, x[1], pos);
		inter[*intindex]->parameter[1] = findRegIndex(x[2]);
	}
	else{
		inter[*intindex]->opcodeno = index + 1;
		inter[*intindex]->parameter[0] = findRegIndex(x[1]);
		inter[*intindex]->parameter[1] = find_in_table(s, x[2], pos);
	}
	*intindex = *intindex + 1;
}

int find_opcode(char *a){
	for (int i = 0; i < 17; i++){
		if (strcmp(a, opcode[i]) == 0){
			return i;
		}
	}
	return -1;
}

void if_insert(int index, char **x, int *intindex, struct intermediate** inter){
	inter[*intindex]->lineno = *intindex;
	inter[*intindex]->opcodeno = index;
	inter[*intindex]->parameter[0] = findRegIndex(x[1]);
	inter[*intindex]->parameter[1] = findRegIndex(x[3]);
	inter[*intindex]->parameter[2] = find_opcode(x[2]);


	*intindex = *intindex + 1;
}

int find_index_label(char c, struct label **l, int *labind){
	for (int i = 0; i < *labind; i++){
		if (l[i]->lab == c){
			return l[i]->seek;
		}
	}
	return -1;
}


void insert_to_intermediate(char **x, int *intindex, struct intermediate** inter, struct symboltable **s, int *ind, struct label **l, int *labind){
	int index = find_opcode(x[0]);

	/*for (int i = 0; i <= 14; i++){
	if (strcmp(x[0], opcode[i]) == 0){
	index = i;
	break;
	}
	}*/
	if ((strcmp(x[0], "READ") == 0) || (strcmp(x[0], "PRINT") == 0)){
		read(index, x, intindex, inter);
	}
	else if ((strcmp(x[0], "ADD") == 0) || (strcmp(x[0], "SUB") == 0) || (strcmp(x[0], "MUL") == 0) || (strcmp(x[0], "DIV") == 0)){
		add(index, x, intindex, inter);
	}
	else if ((strcmp(x[0], "MOV") == 0)){
		mov(index, x, intindex, inter, s, ind);
	}
	else if (x[0][1] == ':'){
		l[*labind]->lab = x[0][0];
		l[*labind]->seek = *intindex;
		*labind = *labind + 1;
	}
	else if ((strcmp(x[0], "JMP") == 0)){
		int z = find_index_label(x[1][0], l, labind);
		inter[*intindex]->lineno = *intindex;
		inter[*intindex]->opcodeno = index;
		inter[*intindex]->parameter[0] = z;
		*intindex = *intindex + 1;
	}
	else if ((strcmp(x[0], "IF") == 0)){
		if_insert(index, x, intindex, inter);
		push(stack, *intindex - 1);
		printf("%d \n", stack[top]);
	}

	else if ((strcmp(x[0], "ELSE") == 0)){

		inter[*intindex]->lineno = *intindex;
		inter[*intindex]->opcodeno = index;
		*intindex = *intindex + 1;
		int d = pop(stack);
		inter[d]->parameter[3] = *intindex;
		push(stack, *intindex - 1);

	}
	else if ((strcmp(x[0], "ENDIF") == 0)){
		int d = pop(stack);
		inter[d]->parameter[0] = *intindex;
	}
}

void menu(char *str, int *arr, struct symboltable** s, int *index, int *pos, int *intindex, struct intermediate **inter, struct label **l, int *labind){

	int k = 0, j = 0;
	char **x = parse(str);
	/*for (int i = 0; i < 3; i++){
	printf("%s ", x[i]);
	}
	*/

	if (strcmp(x[0], "CONST") == 0 || strcmp(x[0], "DATA") == 0){
		insert_to_symbol(x, str, arr, s, index, pos);
	}
	else {
		insert_to_intermediate(x, intindex, inter, s, index, l, labind);
	}
}

void read(int i, int *j, int *arr, struct symboltable** s, int *index, int *pos, int *intindex, struct intermediate **inter, struct label **l, int *labind)
{
	int n;
	scanf("%d", &n);
	regvalue[inter[*j]->parameter[0]] = n;
}

void print(int i, int *j, int *arr, struct symboltable** s, int *index, int *pos, int *intindex, struct intermediate **inter, struct label **l, int *labind)
{
	printf("printing value\n");
	printf("%d", regvalue[inter[*j]->parameter[0]]);
}


void mov0_execute(int i, int *j, int *arr, struct symboltable** s, int *index, int *pos, int *intindex, struct intermediate **inter, struct label **l, int *labind){
	int a, pt;
	if (i == 0){
		pt = regvalue[inter[*j]->parameter[1]];
		//		printf("%d ", pt);
		arr[inter[*j]->parameter[0]] = regvalue[inter[*j]->parameter[1]];
	}
	else{
		pt = arr[inter[*j]->parameter[1]];
		//		printf("%d ", pt);
		regvalue[inter[*j]->parameter[0]] = arr[inter[*j]->parameter[1]];
	}
}

void add_execute(int i, int *j, int *arr, struct symboltable** s, int *index, int *pos, int *intindex, struct intermediate **inter, struct label **l, int *labind){
	int a = regvalue[inter[*j]->parameter[1]];
	int b = regvalue[inter[*j]->parameter[2]];
	int c = a + b;
	regvalue[inter[*j]->parameter[0]] = c;
}
void sub_execute(int i, int *j, int *arr, struct symboltable** s, int *index, int *pos, int *intindex, struct intermediate **inter, struct label **l, int *labind){
	regvalue[inter[*j]->parameter[0]] = regvalue[inter[*j]->parameter[1]] - regvalue[inter[*j]->parameter[2]];
}
void mul_execute(int i, int *j, int *arr, struct symboltable** s, int *index, int *pos, int *intindex, struct intermediate **inter, struct label **l, int *labind){
	regvalue[inter[*j]->parameter[0]] = regvalue[inter[*j]->parameter[1]] * regvalue[inter[*j]->parameter[2]];
}
void div_execute(int i, int *j, int *arr, struct symboltable** s, int *index, int *pos, int *intindex, struct intermediate **inter, struct label **l, int *labind){
	regvalue[inter[*j]->parameter[0]] = regvalue[inter[*j]->parameter[1]] / regvalue[inter[*j]->parameter[2]];
}


void if_execute(int i, int *j, int *arr, struct symboltable** s, int *index, int *pos, int *intindex, struct intermediate **inter, struct label **l, int *labind){
	int a = regvalue[inter[*j]->parameter[0]];
	int b = regvalue[inter[*j]->parameter[1]];

	int flag = 0;

	int cond = inter[*j]->parameter[2];

	if (cond == 8){
		flag = (a == b);
	}
	else if (cond == 9){
		flag = (a < b);
	}
	else if (cond == 10){
		flag = (a > b);
	}
	else if (cond == 11){
		flag = (a <= b);
	}
	else if (cond == 12){
		flag = (a >= b);
	}

	if (flag == 0)
		*j = inter[*j]->parameter[3] - 1;

}


void execution(int *arr, struct symboltable** s, int *index, int *pos, int *intindex, struct intermediate **inter, struct label **l, int *labind){
	for (int i = 0; i < *intindex; i++){
		if (inter[i]->opcodeno == 1 || inter[i]->opcodeno == 0){
			mov0_execute(inter[i]->opcodeno, &i, arr, s, index, pos, intindex, inter, l, labind);
		}
		else if (inter[i]->opcodeno == 14)
			read(inter[i]->opcodeno, &i, arr, s, index, pos, intindex, inter, l, labind);
		else if (inter[i]->opcodeno == 13)
			print(inter[i]->opcodeno, &i, arr, s, index, pos, intindex, inter, l, labind);

		else if (inter[i]->opcodeno == 2)
			add_execute(inter[i]->opcodeno, &i, arr, s, index, pos, intindex, inter, l, labind);
		else if (inter[i]->opcodeno == 3)
			sub_execute(inter[i]->opcodeno, &i, arr, s, index, pos, intindex, inter, l, labind);
		else if (inter[i]->opcodeno == 4)
			mul_execute(inter[i]->opcodeno, &i, arr, s, index, pos, intindex, inter, l, labind);
		else if (inter[i]->opcodeno == 5)
			div_execute(inter[i]->opcodeno, &i, arr, s, index, pos, intindex, inter, l, labind);
		else if (inter[i]->opcodeno == 6)
			i = inter[i]->parameter[0] - 1;
		else if (inter[i]->opcodeno == 7)
			if_execute(inter[i]->opcodeno, &i, arr, s, index, pos, intindex, inter, l, labind);
		else if (inter[i]->opcodeno == 16)
			i = inter[i]->parameter[0] - 1;
	}

}




int main(){
	int *arr = (int*)calloc(100, sizeof(int));

	struct symboltable **s = (struct symboltable**)malloc(sizeof(struct symboltable*) * 100);
	for (int i = 0; i < 100; i++){
		s[i] = (struct symboltable*)malloc(sizeof(struct symboltable));
	}

	struct intermediate **inter = (struct intermediate**)malloc(sizeof(struct intermediate*) * 100);
	for (int i = 0; i < 100; i++){
		inter[i] = (struct intermediate*)malloc(sizeof(struct intermediate));
	}

	struct label **l = (struct label**)malloc(sizeof(struct label*) * 100);
	for (int i = 0; i < 100; i++){
		l[i] = (struct label*)malloc(sizeof(struct label));
	}
	int labind = 0;
	int intindex = 0;
	int index = 0, pos = 0;
	char buff[255];

	/*char **x=parse("ADD BX,AX");
	for (int i = 0; i < 3; i++){
	printf("%s ", x[i]);
	}*/

	FILE *fp = fopen("3.txt", "r");
	while ((fgets(buff, 255, fp)) != NULL){
		menu(buff, arr, s, &index, &pos, &intindex, inter, l, &labind);

	}

	for (int i = 0; i < index; i++){
		printf("%c ", s[i]->name);
		printf("%d ", s[i]->address);
		printf("%d \n", s[i]->size);
	}

	printf("intermediate table\n");
	for (int i = 0; i < intindex; i++){
		printf("%d ", inter[i]->lineno);
		printf("%d ", inter[i]->opcodeno);
		for (int j = 0; j < 6; j++)
			printf("%d ", inter[i]->parameter[j]);
		printf("\n");
	}

	printf("Label index\n");
	for (int i = 0; i < labind; i++){
		printf("%c ", l[i]->lab);
		printf("%d \n", l[i]->seek);
	}


	execution(arr, s, &index, &pos, &intindex, inter, l, &labind);
	printf("\n\n");
	//for (int i = 0; i < 8; i++)
	//	printf("%d ", regvalue[i]);

	fp = fopen("symboltable.bin", "wb");
	FILE *fp1 = fopen("intermediate.bin", "wb");
	FILE *fp2 = fopen("label.bin", "wb");


	fwrite(s, sizeof(struct symboltable), 1, fp);
	fwrite(inter, sizeof(struct intermediate), 1, fp1);
	fwrite(l, sizeof(struct label), 1, fp2);
	fclose(fp);
	fclose(fp1);
	fclose(fp2);

	fp = fopen("symboltable.bin", "rb");
	fp1 = fopen("intermediate.bin", "rb");
	fp2 = fopen("label.bin", "rb");

	struct symboltable **s1 = (struct symboltable**)malloc(sizeof(struct symboltable*) * 100);
	for (int i = 0; i < 100; i++){
		s1[i] = (struct symboltable*)malloc(sizeof(struct symboltable));
	}

	struct intermediate **inter1 = (struct intermediate**)malloc(sizeof(struct intermediate*) * 100);
	for (int i = 0; i < 100; i++){
		inter1[i] = (struct intermediate*)malloc(sizeof(struct intermediate));
	}

	struct label **l1 = (struct label**)malloc(sizeof(struct label*) * 100);
	for (int i = 0; i < 100; i++){
		l1[i] = (struct label*)malloc(sizeof(struct label));
	}

	fread(s1, sizeof(struct symboltable), 1, fp);
	fread(inter1, sizeof(struct intermediate), 1, fp1);
	fread(l1, sizeof(struct label), 1, fp2);
	fclose(fp);
	fclose(fp1);
	fclose(fp2);

	//	execution(arr, s1, &index, &pos, &intindex, inter1, l1, &labind);

	_getch();
	return 0;
}
