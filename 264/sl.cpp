#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <cstdlib>

using namespace std;

//class definition for SimpleList
template <class T>
class SimpleList {

	protected:
		class Node {
	 		public:
				T data;
				Node *next;
				Node(T data);
		};
	
		Node *first, *last;
		string name;
		int size;

	public:
		SimpleList(string name);
		string getName();
		int getSize();

		T pop(); //pop is the same for both stacks and queues
		virtual void push(T data) = 0;
};

// function definitions

template <class T>
SimpleList<T>::Node::Node(T data){
	this ->data = data;
	next = NULL;
}

template <class T>
SimpleList<T>::SimpleList(string name){
	this-> name = name;
	size = 0;
	first = NULL;
	last = NULL;
}

template <class T>
string SimpleList<T>::getName(){
	return name;
}

template <class T>
int SimpleList<T>::getSize(){
	return size;
}

template <class T>
T SimpleList<T>::pop(){
	T temp;

	if (this->getSize() > 1){
		temp = this->first->data;   //stores popped value as temp
		SimpleList<T>::Node* np = this->first;   // temp pointer to top
		first = first->next;
		delete np;//delete old top
		size--;

	} else if(this->getSize() == 1) {
		temp = this->first->data;//stores popped value as temp
		delete first;
		this-> last = this-> first = NULL;
		size--;
	}
	
		return temp;
}

	//class definition for stack
template <class T>
class Stack: public SimpleList<T>{
	public:
		Stack(string name):SimpleList<T>(name){};
		void push(T data);
		
};

//class definition for Queue
template <class T>
class Queue: public SimpleList<T> { 
	public:
		Queue(string name): SimpleList<T>(name){};
		void push(T data);

};

//push for stack
template <class T>
void Stack<T>:: push(T data){
	typename SimpleList<T>::Node *temp = new typename SimpleList<T>::Node(data);
	if (this->getSize() == 0){
		this->first = this->last = temp;
	}
	else{
		temp->next = this->first;
		this->first = temp;
	}
	this->size++;
}

template <class T>
void Queue<T>:: push(T data){
	typename SimpleList<T>::Node *temp = new typename SimpleList<T>::Node(data);
	if (this->getSize() == 0){
		this->first = this->last = temp;
	}
	else{
		this->last->next = temp;
		temp =this->last;
	}
	this->size++;
}


//searching function 
template <class T>
SimpleList<T> *find(string name, list<SimpleList<T> *> *list){
	for(typename list<SimpleList<T> *>::iterator itr = list->begin(); itr != list->end(); itr++){	//iterating through the pointers
		if ((*itr)->getName() == name){ //if the names match
			return (*itr); // if match is found, return the match
		}
	}
	return 0; // if no instance of the string is found, return 0
}

template <class T>
void createList(list<SimpleList<T> *> *list, string name, string typelist, ofstream *out){
	if (find<T>(name, list)){
		(*out) << "ERROR: This name already exists!" << endl;
		return;
	}
	// if no list found, create a list

	SimpleList<T> *ptr;
	if (typelist == "queue")
		ptr = new Queue<T>(name);   //create a new Queue
	else
		ptr = new Stack<T>(name);   //create a new Stack

	list->push_front(ptr);
}

template <class T>
void pushList( list<SimpleList<T>*> *list, string name, T data, ofstream *out){
	SimpleList<T> *ptr = find<T>(name, list);
	if (ptr)
		ptr -> push(data);
	else 
		(*out) << "ERROR: This name does not exist!" << endl;
}

template <class T>
void popList( list<SimpleList<T>*> *list, string name, ofstream *out){
	SimpleList<T> *ptr = find<T>(name, list);
	if (ptr){
		if (ptr -> getSize() != 0)
			(*out) << "Value popped: " << ptr ->pop()<< endl;
		else
			(*out) << "ERROR: This list is empty!"<< endl;
	} 
	else
		(*out) << "ERROR: This name does not exist!" << endl;
} 


int main(int argc, char **argv){
	string filename_in = "";
	string filename_out = "";
	
	ifstream file_in; 
	ofstream file_out;
	
	cout << "Enter name of input file: ";
	getline(cin, filename_in);
	cout << "Enter name of output file: ";
	getline(cin, filename_out);
	
	
	file_in.open(filename_in.c_str());//opens the files
	file_out.open(filename_out.c_str());
	
	
	if (!file_in.is_open()){
		cout << "Cannot open File"<< endl;
		exit(-1);
	}

	list<SimpleList<int> *> listSLi; //all integer stacks and queues
	list<SimpleList<double> *> listSLd; //all double stacks and queues
	list<SimpleList<string> *> listSLs; //all string stacks and queues

	//time to parse the data

	string command;
	char datatype;
	string firstWord, secondWord, thirdWord;

	while (getline(file_in, command)){
		file_out << "PROCESSING COMMAND: "<< command << endl;
		stringstream parse;
		parse << command;
		parse >> firstWord;
		parse >> secondWord;
		datatype = secondWord.at(0);// either i(integer), d(double), or s(string)
	
		if (parse >> thirdWord) {// if the third word word exists, then it has to be create or push
			if (firstWord == "create"){
				if (datatype == 'i')
					createList<int>(&listSLi, secondWord, thirdWord, &file_out);
				else if (datatype == 'd')
					createList<double>(&listSLd, secondWord, thirdWord, &file_out);
				else 
					createList<string>(&listSLs, secondWord, thirdWord, &file_out);
			}
			else {
				if ( datatype == 'i')
					pushList<int>(&listSLi, secondWord, std::stoi(thirdWord.c_str()), &file_out);
				else if ( datatype == 'd')
					pushList<double>(&listSLd, secondWord, std::stod(thirdWord.c_str()), &file_out);
				else
					pushList<string>(&listSLs, secondWord, thirdWord, &file_out);
			}
		}
		else{ // pop
			if (datatype == 'i')
				popList<int>(&listSLi, secondWord, &file_out);
			else if (datatype == 'd')
				popList<double>(&listSLd, secondWord, &file_out);
			else
				popList<string>(&listSLs, secondWord, &file_out);
		}
	}
	file_in.close();
	file_out.close();
}

