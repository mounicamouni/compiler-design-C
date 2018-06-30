#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<conio.h>
#include<malloc.h>
#include<string.h>

char** tokenize(char *str, int *len)
{
	int j = 0, k = 0;
	char **x = (char**)malloc(sizeof(char) * 40);
	for (int i = 0; i < 40; i++){
		x[i] = (char*)malloc(sizeof(char) * 20);
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

	*len = j;
	for (int i = 0; i < *len; i++)
		printf("%s\n", x[i]);
	return x;
}

char * condition(char *x){
	if (strcmp(x, "LT") == 0)
		return "<";
	else if (strcmp(x, "GT") == 0)
		return ">";
	else if (strcmp(x, "GTEQ") == 0)
		return ">=";
	else if (strcmp(x, "LTEQ") == 0)
		return "<=";

}

int main(){
	FILE *fp = fopen("data.txt", "r");
	FILE *fp1 = fopen("compiler_file.cpp", "w");

	char **arr,buff[255];
	int len;

	fprintf(fp1, "#define _CRT_SECURE_NO_WARNINGS\n");
	fprintf(fp1, "#include<stdio.h>\n");
	fprintf(fp1, "#include<malloc.h>\n");
	fprintf(fp1, "#include<string.h>\n");
	
	fprintf(fp1, "int AX,BX,CX,DX,EX,FX,GX,HX;\n ");
	char *cond;
	while (fgets(buff, 255, fp) != NULL){
		len = 0;
		arr = tokenize(buff,&len);


		if (strcmp(arr[0], "START:") == 0){
			fprintf(fp1, "int main(){\n");
		}
		else if (strcmp(arr[0],"DATA")==0){
			fprintf(fp1, "int %s ;\n", arr[1]);
		}
		else if (strcmp(arr[0], "CONST") == 0){
			fprintf(fp1, "const int %s = %s;\n", arr[1], arr[2]);
		}
		else if (strcmp(arr[0], "READ") == 0){
			fprintf(fp1, "scanf(\"%%d \",&%s );\n", arr[1]);
		}
		else if (strcmp(arr[0], "PRINT") == 0){
			fprintf(fp1, "printf(\"%%d\",%s);\n",arr[1]);
		}
		else if (strcmp(arr[0], "MOV") == 0){
			fprintf(fp1, "%s = %s;\n", arr[1], arr[2]);
		}
		else if (strcmp(arr[0], "ADD") == 0){
			fprintf(fp1, "%s = %s + %s;\n", arr[1], arr[2],arr[3]);
		}
		else if (strcmp(arr[0], "SUB") == 0){
			fprintf(fp1, "%s = %s - %s;\n", arr[1], arr[2], arr[3]);

		}
		else if (strcmp(arr[0], "DIV") == 0){
			fprintf(fp1, "%s = %s / %s;\n", arr[1], arr[2], arr[3]);
		}
		else if (strcmp(arr[0], "MUL") == 0){
			fprintf(fp1, "%s = %s * %s;\n", arr[1], arr[2], arr[3]);
		}
		else if (strcmp(arr[0], "IF") == 0){
			cond = condition(arr[2]);
			fprintf(fp1, "if ( %s  %s  %s )\n", arr[1], cond, arr[3]);
		}
		else if (strcmp(arr[0], "THEN") == 0){
			fprintf(fp1, "{\n");
		}
		else if (strcmp(arr[0], "ELSE") == 0){
			fprintf(fp1, "}else{ \n");
		}
		else if (strcmp(arr[0], "ENDIF") == 0){
			fprintf(fp1, "}\n");
		}
		else if (strcmp(arr[0], "JMP") == 0){
			fprintf(fp1, "goto %s;\n",arr[1]);
		}
		else if (arr[0][1] == ':'){
			fprintf(fp1, "%s\n", arr[0]);
		}

	}
	fprintf(fp1, "return 0;\n}\n");
	fclose(fp);
	fclose(fp1);
}