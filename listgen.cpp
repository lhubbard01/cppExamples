//Lyle Hubbard , September 16, 2021

//example file for new c++ users. demonstrates more intermediate level practices
//that are still essential for bug free production. Topics include namespaces, 
//class private structures, wrappers for recursive functions, and inlining.
#include <cstdlib>
#include <iostream>
#include <cstdio>

//Why only these being scoped instead of using namespace std? 
//enables piecemeal approach. similar to python's "from x import y" to
//let users say, for example, "array" instead of "np.array"

using std::cout;
using std::endl;

class LLL
{
  private:
    //Why might this ever be useful? class private structures can act as utilities to implement 
		//the data structure, while avoiding the pollution of the encompassing namespace!
		//Note, this can also be achieved with something termed an anonymous namespace. E.g. "namespace {...}" i.e. without the label!
		struct Node{
      int data;
      Node * next;
    };
		
    Node * head;
		//recursive implementations should be made private    
    
    int insert(Node *& curr, int datum)
		{
      if (!curr)
			{
        curr = new Node;
        curr->next = NULL;
				curr->data = datum; 
				//One can actually have methods tied to structs in c++,
				//though this is just demonstrative... what would be a good method here?

        return 0; 
			}
  
      return insert(curr->next, datum) + 1;
    }


		//It is useful to split sections up according to 
		//sections performing similar subproblems (this is very much 
		//left up to interpretation, though one will get a feel for this over time)
    int remove_all_but(Node *& curr, int sum)
    {
      if (!curr->next) // is read if not current arrow next, i.e. if the next element is null
        return sum;
      
      Node * temp;

      temp = curr->next;
      sum += curr->data;
      delete curr;

      sum = remove_all_but(temp, sum);
      curr = temp;
      return sum;
    }
  

  
	public:
    LLL() : head(NULL){ }

    int insert(int datum)
		{
      return insert(head, datum);
    }

		
		// convention for style is not universally consistent, 
		// but you better make sure yours is consistent with itself!
		// though there are several practices more popular than others, (this can be gathered from, e.g., github)
		// ultimately, the goal should be readability first.
    int remove_all_but()
		{
      if (!head)						return 0;
      else if (!head->next) return 0;
      else
				return remove_all_but(head->next, 0);
    }
    

    
		
		void display()
		{
      if (!head)
			{
        cout << "->|" << endl;
        return;
      }

      Node * curr = head;
      while (curr->next)
			{
        cout << curr->data << "->";
        curr = curr->next;
      }
        
			cout << curr->data;
      cout << "->|" << endl;
    }
};


//Contained within are sections of preprocessor directives. If one were to desire different
//behaviors determined at compile-time, say for a debug build, having code sections wrapped 
//with #ifdef X would enable such behavior. It is good practice to add a comment to each 
//conditionals termination (since visually scoping these can be rather difficult)

void
initRand(unsigned int seed)
{
#ifdef DEBUG
	printf("UNSIGNED SEED: %ul\n", seed);
#endif // DEBUG
	srand(seed);
}



//Inlining is the practice of moving the implementation of a function 
//To all locations where it is called. The author will do this
//to speed up execution (since costly jumps are avoided, as the code
//being called in the source is actually directly subsequent in assembly land)
//This ends up trading memory for speed. 
//In operating systems, this is quite common, as well as embedded systems, time critical apps,etc.
#if DEBUG
inline int 
randDiscrete(int lower, int upper)
{
  int t1 = (upper - lower);
  int t2 = rand() % t1;
  int t3 = lower + t2;
  cout << "step 1) upper - lower " << t1 << "\nstep 2) rand() % (upper - lower) " << t2 << "\nstep 3) lower + (rand() % (upper - lower)) " << t3 << endl;
  return t3;
}

#else
inline int 
randDiscrete(int lower, int upper)
{
    return lower + (rand() % (upper - lower));
}
#endif // DEBUG




//int argc is the number of arguments passed to the runtime. 
//char ** argv is array of arguments. The assembly for a c language runtime
//typically has a start method, which initializes the runtime in user space.
//The first argument, i.e. argv[0], is the name of the executable! 
//argc is important at runtime for grabbing flags, since there can potentially be many
//and their count is not determined until runtime. Without it, one wouldnt know where 
//to stop in reading argv, resulting in a segfault!


int 
main(int argc, char** argv)
{
  int length, lower, upper;
#ifdef CORRECT
  if (argc >= 4)
  {
    length = atoi(argv[1]); // atoi is a cstdlib function that converts strings of integers to integer datatype.
    lower  = atoi(argv[2]);
    cout << "lower: " << lower << endl;
    upper  = atoi(argv[3]);
  }
  else{
    cout << "list gen requires 3 arguments: \n"
    << "\t1) length of generated list \n"
    << "\t2) lower bound of random integer generation\n\t3) upper bound of random integer generation" << endl;
 
    return EXIT_FAILURE;
  }
#else
  length = atoi(argv[1]); // atoi is a cstdlib function that converts strings of integers to integer datatype.
  lower  = atoi(argv[2]);
  upper  = atoi(argv[3]);

#endif // CORRECT


  LLL list;
	initRand(time(NULL)); //time(NULL) grabs the current system time in seconds (since epoch, i.e. Jan 1, 1970, 12:00 AM UTC) 
	//and returns an unsigned 32 bit integer.
	
  
  int input = 0, sum_ins = 0;

  

	for (int i = 0; i < length; ++i)
	{
    input = randDiscrete(lower, upper); //input = lower + (rand()%(upper - lower));
    list.insert(input);
    
		if ((i > 0) && (i < length - 1))
      sum_ins += input;

  }
  list.display();

  int sum_of_removed = list.remove_all_but();
  cout << "Sum of removed integers " << sum_of_removed << "\nSum of input values not head or tail: " << sum_ins << endl;

  list.display();
  return EXIT_SUCCESS; // i.e. 0. This is classically how programs signal a successful exit, hence the name.
	//Other integers can be retirned to signal different error states resulting the exit, or can return an output calculation, etc.
}
