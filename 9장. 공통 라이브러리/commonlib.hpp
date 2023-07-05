#pragma once
#include <tree.h>

//itoa에서 사용 
char tbuf[32];
char bchars[]={'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

namespace std{
	class String{
		.....
		size_t size() const;
		const char* data() const;
		const char* c_str() const;
		
		const char& operator[](size_t index) const;
		char* operator[](size_t index);
		
		private:
			char* data_;
			size_t len_;
	};
	
	template <class T>
	class vector{//스택구현시 상속하여 사용 
		.....
		unsigned int capacity() const;
		unsigned int size() const;
		bool empty() const;
		Iterator begin();
		Iterator end();
		T& front();
		T& back();
		void push_back(const T& value);
		void pop_back();
		
		void reserve(unsigned int capacity);
		void resize(unsigned int size);
		
		T& operator[](unsigned int index);
		vector<T>& operator=(const Vector<T>&);
		void clear();
		
		private:
			unsigned int _size;
			unsigned int _capacity;
			unsigned int Log;
			T* buffer;
	};
	
	template <class Key, class T, class Compare=less<Key>, class Alloc=alloc>
	class map{
		//begin, end, rbegin, rend, empty, size
		iterator begin(){ return t.begin(); }
		iterator end(){ return t.end(); }
		reverse_iterator rbegin(){ return t.rbegin(); }
		reverse_iterator rend(){ return t.rend(); }
		bool empty() const{ return t.empty(); }
		size_type size() const { return t.size(); }
		
		//입력, 삭제 관련
		pair<iterator, bool> insert(cosnt value_type& x){ return t.insert_unique(x); }
		void erase(iterator position){ t.erase(position); }
		void clear(){ t.clear(); }
		
		//찾기
		iterator find(const key_type& x){ return t.find(x); }
		..... 
	};
	
	template <typename T>
	class deque{
		public:
			.....
			int size();
			bool empty() const;
			void push_front(const T& b);
			void push_back(const T& b);
			T& front();
			T& back();
			void pop_front();
			void pop_back();
			.....
		private:
			darray<T> _deque_front;
			darray<T> _deque_rear;
			.....
	};
	
	const char endl='\n';
	class ostream{
		public:
			ostream& operator<<(char *cp);
			ostream& operator<<(char c);
			ostream& operator<<(int value);
			ostream& operator<<(unsinged char *cq);
			ostream& operator<<(unsigned char c1);
			ostream& ostream::operator<<(const char *cp);
		private:
			
	};
	extern ostream cout;//보통 공통라이브러리 프로젝트에는 구현부가 없고, 유저 프로젝트나 커널 프로젝트에 구현부가 존재하기에 extern으로 선언한다. 
	
	struct ListNode{
		public:
			inline ListNode();
			virtual ~ListNode(){}
			inline void RemoveFromList();
			LPVOID _data;
		private:
			ListNode *fNext;
			ListNode *fPrev;
			friend class DoubleLinkedList;
			friend class SkyQueue;
	}; 
	
	struct DoubleLinkedList{//ProcessManager::RemoveFromTaskList(Process* pProcess)함수에서 사용예정, 큐구현시 상속하여 사용 
		public:
			inline DoubleLindkedList();
			inline ListNode* AddToTail(ListNode*);
			inline ListNode* AddToHead(ListNode*);
			inline ListNode* AddBefore(ListNode *next, ListNode* newEntry);
			inline ListNode* AddAfter(ListNode *prev, ListNode *newEntry);
			inline ListNode* Remove(ListNode*);
			inline ListNode* Remove(void*);
			inline void Clear();
			inline ListNode* GetHead() const;
			inline ListNode* GetTail() const;
			inline ListNode* GetNext(const ListNode*) const;
			inline ListNode* GetPrevious(const ListNode*) const;
			inline bool IsEmpty() const;
			inline int CountItems() const;
		
		protected:
			ListNode fDummyHead;
	};
	
} 

static void *allocate(size_t n);
static void deallocate(void *p, size_t n);
