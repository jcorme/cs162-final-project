# Final Project - Escape from CS 162

**Due at 11:59pm on Monday, 3/19/2017**

In this final project, you'll implement a game that combines many of the things we talked about in class this year.  The goal of the game will be to "escape" from CS 162 with a passing grade.

## Escape from CS 162

In the game you'll implement for this project, CS 162 is represented as a multi-level maze.  The player's goal is to guide an Intrepid Student through the maze, picking up Programming Skills along the way.  As the Intrepid Student makes their way through the maze, climbing ladders to reach higher and higher levels, they must either avoid the TAs, who are also wandering through the maze, or else appease the TAs by demonstrating a Programming Skill.  On the highest level of the maze, the Intrepid Student will encounter the Instructor, and when they do, they must have picked up enough Programming Skills to satisfy the Instructor (skills demonstrated to the TAs don't satisfy the Instructor).  If the Instructor is satisfied by the Programming Skills picked up by the Intrepid Student, the Intrepid Student is allowed to escape from the CS 162 maze with a passing grade.  Otherwise, they must repeat the whole maze from the beginning (ugh)!

## Components of the game

### The maze

The configuration of the maze for the game must be read into the program from a file, which should be specified as a command line argument when your program is started.  For example, if your program is named `escape`, you would start your program like this:
```
./escape maze.txt
```

The `maze.txt` file will begin with one line specifying the size of the maze.  This line will contain three values:
```
NUM_LEVELS LEVEL_HEIGHT LEVEL_WIDTH
```
After this line, there will be `NUM_LEVELS * LEVEL_HEIGHT` additional lines, each containing `LEVEL_WIDTH` characters.  These lines each represent one row of the maze, and they can contain the following characters:
  * `' '` (space) - an open space in the maze, which may be occupied by the Intrepid Student and the TAs
  * `'#'` - a wall in the maze, which may *not* be occupied by the Intrepid Student and the TAs
  * `'@'` - the beginning location of the Intrepid Student on this level of the maze; this space may be occupied by the Intrepid Student and the TAs
  * `'^'` - the ladder for this level of the maze, which leads up to the next level; the ladder space may be occupied by the Intrepid Student and the TAs
  * `'%'` - the location of the Instructor (only on the final level); the Intrepid Student and the TAs may occupy the same space as the Instructor

There is an example `maze.txt` file included in this repo that you can use for testing.

In addition to the things in this file-specified configuration, the maze will contain the following things, which should be generated at runtime:
  * The Intrepid Student, whose location begins at the location specified by the `'@'` character and changes as the player moves around the maze.  In the game, use the `'*'` character to represent the Intrepid Student.
  * Two TAs per level, whose locations are randomly selected from among the open spaces in the maze at the beginning of each level.  In the game, use the `'T'` character to represent a TA.
  * Three Programming Skills per level, whose locations are also randomly selected from among the open spaces in the maze at the beginning of each level.  In the game, use the `'$'` character to represent a Programming Skill.

### The Intrepid Student

Each turn of the game, the Intrepid Student can take one of the following actions:

  * **Move**: take a step into any open/occupiable space in the maze.  The player specifies which direction to move by entering one of these keys:
    * `'W'` - move up
    * `'A'` - move left
    * `'S'` - move down
    * `'D'` - move right

  * **Demonstrate a Programming Skill**: The player may choose to have the Intrepid Student demonstrate a Programming Skill by using the `'P'` key.  More info is below on what it means to demonstrate a programming skill.

  * **Climb up the ladder**: If the Intrepid Student is on the space where the ladder for the current level is, the player may use the `'U'` key to climb up the ladder to the next level, where they begin at the location specified by the `'@'` symbol in the maze file.  The Intrepid Student may not climb back down the ladder.

The Intrepid Student will pick up Programming Skills as they traverse the CS 162 maze.  These Programming Skills can be represented by a counter that is incremented each time one is picked up and decremented each time one is demonstrated.

### The TAs

The TAs wander randomly around the CS 162 maze, trying to catch students to make sure they know how to program.  Each turn of the game, each TA moves randomly into an open space in the maze adjacent to their current location.

If a TA occupies the same space as the Intrepid Student or a space *adjacent* to the space occupied by the Intrepid Student, the Intrepid Student is caught, and the stress of being caught causes them to lose all of their Programming Skills (i.e. the Programming Skills counter is reset to 0).  If this happens, the Intrepid Student is brought back to the beginning location for the current level, and the level is reset with two randomly placed TAs and three randomly placed Programming Skills.

However, if the Intrepid Student demonstrates a Programming Skill before a TA enters the same or an adjacent space, then all of the TAs on the current level of the maze are appeased for 10 turns.  While the TAs are appeased, they will still move, but they will ignore the Intrepid Student and not catch them.  If the TAs are appeased, the Intrepid Student may occupy the same space as a TA.  If the Intrepid Student climbs to a new level in the maze, the TAs in that level are not appeased, even if the TAs on the previous level were.

### Programming Skills

Programming Skills are placed in a random open space (*not* in a space containing the Instructor or one containing a ladder) and do not move.  If the Intrepid Student enters a space where a Programming Skill lies, then they pick up that Programming Skill, and it disappears from the maze.  If a TA enters a space where a Programming Skill lies, they ignore the Skill (because they already possess all of the Skills), and the Skill remains in the same location when the TA moves on.

### The Instructor

The only way the Intrepid Student can escape from the CS 162 maze is by satisfying the Instructor, who lives in the last level of the maze and does not move.  To satisfy the Instructor, the Intrepid Student must enter a space adjacent to the Instructor's location while holding at least three Programming Skills.  If the Intrepid Student satisfies the Instructor, they are given a passing grade and allowed to exit the CS 162 maze.  *However*, if the Intrepid Student enters a space adjacent to the Instructor *without* holding at least three Programming Skills, they are given a *failing* grade and must complete the CS 162 maze again, starting from the beginning of the first level.

## Program design

You must design and implement the following classes in your program.

### The `MazeLocation` class

This class represents a generic location in the maze.  It should be an *abstract* class and have the following public methods:
  * `bool is_occupiable()` - indicates whether the space can be occupied by the Intrepid Student or the TAs
  * `char get_display_character()` - returns the character that should be displayed in the console to represent this location in the maze

You may add other methods as needed, along with any appropriate private or protected data members.

### The `Wall` class

This class should inherit from the `MazeLocation` class to represent a wall in the maze.  A wall is not occupiable, and its display character is always `'#'`.  You may add methods as needed, along with any appropriate private or protected data members.

### The `OpenSpace` class

This class should also inherit from the `MazeLocation` class to represent an open space in the maze.  An open space is always occupiable, but its display character may change depending on whether the space is occupied by the Intrepid Student, a TA, or the Instructor, or whether there is a ladder or a Programming Skill in the space.

You should implement additional methods for this class, as well, including:
  * `void set_has_student(bool)`,  `bool has_student()` - allow the caller to set whether or not this space contains the Intrepid Student and ask whether the space contains the Intrepid Student
  * `void set_has_ta(bool)`,  `bool has_ta()` - allow the caller to set whether or not this space contains a TA and ask whether the space contains a TA
  * `void set_has_instructor(bool)`,  `bool has_instructor()` - allow the caller to set whether or not this space contains the Instructor and ask whether the space contains the Instructor
  * `void set_has_ladder(bool)`,  `bool has_ladder()` - allow the caller to set whether or not this space contains a Ladder and ask whether the space contains a Ladder
  * `void set_has_skill(bool)`,  `bool has_skill()` - allow the caller to set whether or not this space contains a Programming Skill and ask whether the space contains a Programming Skill

You may add other methods as needed, along with any appropriate private or protected data members.

### The `MazeLevel` class

This class represents a single level of the CS 162 maze.  In this class, you must represent the grid of locations as a 2D vector of `Wall` and `OpenSpace` objects.  In order to implement this vector, you'll have to use polymorphism, which means it will have to be a 2D vector of `MazeLocation` pointers:
```
std::vector<std::vector<MazeLocation*> > locations;
```
You can add other private or protected members as needed.

Your `MazeLevel` class should contain at least the following methods:
  * `MazeLevel(std::ifstream&, int, int)` - the constructor for this class should take the level's height and width as arguments along with an open file stream from which to read the configuration of the level, and it should allocate and initialize the grid of locations using the information it reads from the file.  Remember, because the grid is represented as a 2D vector of `MazeLocation` pointers, you'll have to dynamically allocate each individual location itself, e.g.:
  ```
  this->locations[i][j] = new Wall();
  ```
  or
  ```
  this->locations[i][j] = new OpenSpace();
  ```

  * `~MazeLevel()` - because you're allocating space in the constructor, you'll have to make sure to free it all in the destructor

  * `MazeLocation* get_location(int, int)` - returns the `MazeLocation` at a specified row and column of the grid of locations

You can add other methods to this class as needed.

### The `MazePerson` class

This class represents a generic person in the maze.  It should be an *abstract* class with the following public methods:
  * `char get_move()` - this method will be overridden by the classes below to get a direction in which to move the person; the direction should be represented by a character code (like the characters the player chooses to indicate which direction to move the Intrepid Student)
  * `void set_location(int, int)` - allow the caller to set the location of this person
  * `int get_row()`, `int get_col()` - allow the caller to get the row and column of this person's location

You can add other methods to this class as needed, along with any appropriate data members.


### The `IntrepidStudent` class

This class should inherit from the `MazePerson` class to represent the Intrepid Student.  The overridden `get_move()` method for this class should prompt the player to perform their action for the turn by selecting `'W'`, `'A'`, `'S'`, `'D'`, `'U'`, or `'P'`.  In addition, you'll have to keep track of how many Programming Skills the Intrepid Student is holding and have methods for incrementing/decrementing the number of Programming Skills as well as getting the number available.  You can add other methods to this class as needed, along with any appropriate data members.

### The `TA` class

This class should also inherit from the `MazePerson` class to represent a TA.  The overridden `get_move()` method for this class should select a random direction in which to move.  In addition, you'll need to keep track of whether this TA is currently appeased and, if so, how many turns of being appeased they have remaining.  You'll probably at least need an accessor to return `true` or `false` depending on whether or not the TA is appeased.  You can add other methods as needed, along with any appropriate data members.

### The `Instructor` class

This class, too, should inherit from the `MazePerson` class to represent the Instructor.  This should be a pretty simple class.  Its `get_move()` method should return no move every time.  You can add other methods as needed, along with any appropriate data members.

### The `Maze` class

Finally, you should implement a class to represent the entire CS 162 maze.  This class should contain the levels of the maze as well as all of the people in it.  Interactions with the maze from the `main()` function should be handled via the methods of this class.

Importantly, as moves are selected using the `get_move()` method from each of the `MazePerson` classes, you'll have to make sure that those moves don't cause people to move onto spaces that are not occupiable or outside the maze entirely.  To accomplish this, think about how you can use the classes derived from the `MazePerson` class polymorphically within your `Maze` class to make your life easier.  Note that you may have to call `get_move()` multiple times to obtain a valid move.

### Additional program requirements

Here are some additional requirements:

  * Each class should be implemented in separate `.hpp` and `.cpp` files, and you should have a single application `.cpp` file.

  * You should write a makefile to compile your program.

  * Your program cannot have any memory leaks.

## Reflection document

In addition to your program, part of your grade for the project will be based on a reflection document you'll write about your experiences while working on your program.  Your reflection document should be named `REFLECTION.txt` and be included in your git repo.  In this document, you should talk about the *process* of writing your code.  For example, you can answer questions like these in your reflection document:

  * What decisions or assumptions did you make about how to design your program?

  * What was your test plan for your program?

  * How did your testing work out?  What bugs did you uncover through testing?  How did you fix those bugs?

  * What problems or obstacles did you run into while you were developing your program?  How did you resolve these?

  * What resources were useful to you in solving problems you ran into?  Were there helpful websites or Stack Overflow posts?  What help did you get from the TAs and/or the Instructor?  Were specific parts of the course notes helpful?

Be as thorough as you can when writing your reflection document.  You won't be demoing your final project with the grader, so your reflection document should provide answers to the kinds of questions you might expect your grader to ask.  If something in your program isn't working correctly, and the grader can't figure out why you made certain choices, it's likely you'll lose more points than you might have if the grader understood your design choices, so use the reflection document to make sure your grader is informed.

## Code style

You must include a header comment for each source code file that contains a description of the file (including how to run the program, command line arguments, etc. if the file contains your `main()` function), your name, and the date.  Your code should be well commented, including header comments for *all* functions describing what the function does, its parameters, and any pre- and post-conditions for the function.  You should appropriately use whitespace, newlines, and indentation.

Make sure you review the style guidelines for the course, and start trying to follow them:

http://web.engr.oregonstate.edu/~hessro/static/media/162_style_guideline.4812c1d9.pdf

## Submitting your program

To submit your program, you need to make sure the following files are committed into your git repository and pushed to your `master` branch on GitHub before the due date above:
  * The `.cpp` file containing your application code.
  * All of the `.hpp` and `.cpp` files containing the interface and implementation of your classes.
  * Your `Makefile`.
  * Your `REFLECTION.txt`.

Do not commit any other files (other than the ones that were already in your repository at the start of the assignment).  A good way to check whether your assignment is submitted is to simply look at your repo on GitHub (i.e. https://github.com/OSU-CS162-W18/final-project-YourGitHubUsername).  If your files appear there before the deadline, they they are submitted.

## Grading criteria

Your program **MUST** compile and run on `flip.engr.oregonstate.edu`, so make sure you have tested your work there before you make your final submission, since a program that compiles and runs in one environment may not compile and run in another.  **Assignments that do not compile on `flip` will receive a grade of 0.**  If you do not have an ENGR account, you can create one at https://teach.engr.oregonstate.edu/.

This assignment is worth 100 points total:
  * 10 points: your code is appropriately commented and uses consistent and appropriate style
  * 15 points: your `REFLECTION.txt` document thoughtfully explains your work on your project
  * 15 points: your program correctly reads maze levels from a maze configuration file and creates corresponding `MazeLevel` objects, including error checking on the input file
  * 10 points: the `Wall` and `OpenSpace` classes are correctly implemented and derive from the (correctly implemented) `MazeLocation` class
  * 10 points: the `IntrepidStudent`, `TA`, and `Instructor` classes are correctly implemented and derive from the (correctly implemented) `MazePerson` class
  * 10 points: objects of the `IntrepidStudent`, `TA`, and `Instructor` classes correctly move around the maze
  * 5 points: climbing a ladder brings the Intrepid Student to the next level in the maze
  * 5 points: the Intrepid Student picks up a Programming Skill when moving into a space containing one, while TAs do not affect Programming Skills
  * 5 points: if a TA enters a space adjacent to the Intrepid Student and the TA is not appeased, the level is reset, and the Intrepid Student starts from the beginning of the level after losing all Programming Skills
  * 5 points: demonstrating a Programming Skill appeases the TAs on the current level for 10 turns
  * 5 points: if the Intrepid Student reaches the Instructor and holds at least 3 Programming Skills, they are allowed to escape from the maze with a passing grade; otherwise they must repeat the entire maze from the beginning
  * 5 points: all dynamically allocated memory is correctly freed (no memory leaks)

Segmentation faults and other similar errors will result in a 20 point deduction.  **Remember, you won't demo this project for a grade.**
