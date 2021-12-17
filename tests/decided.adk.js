// All this syntax is final.

#include module as module 
#include module //copy-paste thing

if 0 == 0 {
	//do something
}
// I mean we could do something like #include something as variable
// Then maybe they could also do #include something if they don't want to do that -> yes
// yes, but I was also thinking that we need to have some special variable that inlcudes everything in the global scope, so that you could do something like `globalscope.has("this var") or globalscope[var]` but we can add that 

// We also need some kind of way to make modules in other languages to gain functionality that Aardvark doesn't have built in.
// that's kind why i have a file called aardbark.h so it could be included in a c++ file and compiled as a dynamic library. It would be similar to CPython API
//Yes, thats what I was thinking.
//We could make modules in C++ to add functionality that the base-language doesn't have, like sockets or windows, or stuff like that. That way not everything has to be built-in
if 0==0 || 9==8 {
	// Do you need any more information?
}else if 8!=7 {
	
} else {
	
}

// I guess you are right.
//https://www.pythonlikeyoumeanit.com/Module4_OOP/Special_Methods.html for ideas
class Test {
  ~init { //parenthese are not required if it takes no arguments
    this.x = 0
  }
	~ref {
		//called everytime this is referenced
	}
	~string {
		//called when it is being turned into a string. like __repr__ in python
	}
  funct increment() {
    return this.x = 4; // This returns 4 and assigns 4 to this.x. 
  }
}

static b = 0; // static, can not be changed
myvar = 8
c = reference b //c only references b, changing one changes the other

//Match case works like switch case.
a="hi"
match a {
	case "hi" {
		output("it says hi!")
		break
	}
	if .startswith("h") {
		output("Its not hi, but it starts with h")
	}
	default {
		
	}
}
// ; is not required to end lines, but it can be used as a line-break