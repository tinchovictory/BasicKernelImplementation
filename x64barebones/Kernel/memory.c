static char * position = 0x1000000;

char * malloc(long int bytes){
	char * aux = position;
	position+=bytes;
	return aux;
}

char * getPosition(){
	return position;
}