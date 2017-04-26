#include <iostream>
#include <fstream>
#include <sstream>
#include <list>

using namespace std;

template <class T> class SimpleList{
	protected:
		class Node{
			public:
				Node(T data);
				T data;
				Node *next;
		};
		Node *first, *last;
	public:
		SimpleList(string name);
		T pop(); // pop works differently for stack and queue
		virtual void push(T data) = 0;
		string name;
		int size;
};

template<class T> SimpleList<T>::Node::Node(T data){ // Constructor
	this->data = data;
	next = NULL;
}

template<class T> SimpleList<T>::SimpleList(string name){ // Constructor
	this->name = name;
	size = 0;
	first = last = NULL;
}

template<class T> T SimpleList<T>::pop(){
	if(size){
		T ret = first->data;
		Node *remove = first;
		first = first->next;
		delete remove;
		if(!(size--)) last == NULL; // If there is 0 after pop, set last to NULL as well (not needed though)
		return ret;
	}
	return (T)NULL;
}

template <class T> class Stack: public SimpleList<T> {
	public:
		Stack(string name):SimpleList<T>(name){}; // Constructor; does the same as base class's
		void push(T data);
};

template <class T> void Stack<T>::push(T data){
	typename SimpleList<T>::Node *tmp = new typename SimpleList<T>::Node(data);
	if(this->size++)
		tmp->next = this->first;
	else
		this->last = tmp;
	this->first = tmp;
}

template <class T> class Queue: public SimpleList<T> {
	public:
		Queue(string name):SimpleList<T>(name){}; // Constructor; does the same as base class's
		void push(T data);
};

template <class T> void Queue<T>::push(T data){
	typename SimpleList<T>::Node *tmp = new typename SimpleList<T>::Node(data);
	if(this->size++)
		this->last->next = tmp;
	else
		this->first = tmp;
	this->last = tmp;
}

template<class T> SimpleList<T> *search(list<SimpleList<T> *> *mlist, string name){
	for(typename list<SimpleList<T> *>::iterator ci = mlist->begin(); ci != mlist->end(); ci++)
		if((*ci)->name == name)
			return (*ci);
	return NULL;
}

template<class T> void createList(list<SimpleList<T> *> *list, string name, string type, ofstream *out){
	if(search<T>(list, name)){
		(*out) << "ERROR: This name already exists!" << endl;
		return;
	}

	SimpleList<T> *tmp;
	if(type == "queue")
		tmp = new Queue<T>(name);
	else
		tmp = new Stack<T>(name);
	list->push_back(tmp);
}

template<class T> void pushList(list<SimpleList<T> *> *list, string name, T data, ofstream *out){
	if(SimpleList<T> *tmp = search<T>(list, name))
		tmp->push(data);
	else
		(*out) << "ERROR: This name does not exist!" << endl;
}

template<class T> void popList(list<SimpleList<T> *> *list, string name, ofstream *out){
	if(SimpleList<T> *tmp = search<T>(list, name))
		if(tmp->size) // if size > 0 (more specifically, if size != 0)
			(*out) << "Value popped: " << tmp->pop() << endl;
		else
			(*out) << "ERROR: This list is empty!" << endl;
	else // nothing returned from search
		(*out) << "ERROR: This name does not exist!" << endl;
}

int main(int argc, char **argv){
	string filename_in = "", filename_out = "";
	ifstream file_in;
	ofstream file_out;

	cout << "Enter name of input file: ";
	getline(cin, filename_in);
	cout << "Enter name of output file: ";
	getline(cin, filename_out);
	file_in.open(filename_in.c_str());
	file_out.open(filename_out.c_str());

	if(!file_in.is_open()){ // Check to make sure valid input filename
		cout << "File cannot be opened!" << endl;
		exit(-1);
	}

	list<SimpleList<int> *> listSLi; // all integer stacks and queues
	list<SimpleList<double> *> listSLd; // all double stacks and queues
	list<SimpleList<string> *> listSLs; // all string stacks and queues

	string command, operation, name, type_val;
	char datatype;

	while(getline(file_in, command)){
		file_out << "PROCESSING COMMAND: " << command << endl;
		stringstream parser; // stringstream can parse (tokenize with space delimiters) string
		parser << command;
		parser >> operation >> name;
		datatype = name.at(0);

		if(parser >> type_val){
			if(operation == "create"){
				switch(datatype){
					case 'i': createList<int>(&listSLi, name, type_val, &file_out); break;
					case 'd': createList<double>(&listSLd, name, type_val, &file_out); break;
					case 's': createList<string>(&listSLs, name, type_val, &file_out);
				}
			}else{ // assume command was valid and operation was "push"
				switch(datatype){
					case 'i': pushList<int>(&listSLi, name, atoi(type_val.c_str()), &file_out); break;
					case 'd': pushList<double>(&listSLd, name, strtod(type_val.c_str(), 0), &file_out); break;
					case 's': pushList<string>(&listSLs, name, type_val, &file_out);
				}
			}
		}else{ // Condition is false if no third operand; "pop"
				switch(datatype){
					case 'i': popList<int>(&listSLi, name, &file_out); break;
					case 'd': popList<double>(&listSLd, name, &file_out); break;
					case 's': popList<string>(&listSLs, name, &file_out);
				}
		}
	}
	file_in.close();
	file_out.close();
	exit(0);
}
