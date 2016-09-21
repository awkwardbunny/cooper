char *my_itoa(int val, char *result, int base){

	int length = 0;

	unsigned int i = val;

	//absolute
	if(base == 10 && val < 0){
		i = val * -1;
	}

	//divide and calculate from least-significant-digit
	//do-while: loop at least once. (when i = 0, we still want an output)
	do{
		int digit = i % base;
		if(digit < 10)
			digit += '0';
		else
			digit += 'a' - 10;
		result[length] = digit;
		i /= base;
		length++;
	}while(i > 0);

	//dont forget the '-'
	if(base == 10 && val < 0)
		result[length++] = '-';

	//NULL terminator!
	result[length] = 0;

	//reverse the string!!!
	i = 0;
	while(i < length/2){
		char c = result[i];
		result[i] = result[length-i-1];
		result[length-i-1] = c;
		i++;
	}

	return result;
}
