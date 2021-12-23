// All this syntax is final.
#include module as module 
#include module //copy-paste thing
//jump loop
counter = 0
counter++
output(counter)
if counter < 10 #jump 6
#define fn as funct //allows the syntax shown on the next line.
fn a() {
	//works just like funct
}
if 0 == 0 {
	//do something
}
if 0==0 || 9==8 {
	// Do you need any more information?
}else if 8!=7 {
	
} else {
	

array=[0, 1, 2, 3, 4, 5, 6, 7, 8]
array[0:] //whole array
array[0] // first element
array[:5]// all elements up to and including the sixth
array[5:]//all elements past the sixth (not inclusive)
array[-1] //last element

// block merging
if test == "yes" while x > 5 {

}
// Equivalent too
if test == "yes" {
  while x > 5 {
    
  }
}

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
	~get(attribute) {
		//Called when an attribute is being retrieved
	}
	~call(...args) {
		//Called when the instance is being called like a function
	}
	getter funct y() {
		//getter functions are called to get the value of a attribute
	}
  static funct increment() {
    return this.x = 4; // This returns 4 and assigns 4 to this.x. yes
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
	if .startswith("h") {//.startswith is to be added
		output("Its not hi, but it starts with h")
	}
	default {
		
	}
}
// ; is not required to end lines, but it can be used as a line-break