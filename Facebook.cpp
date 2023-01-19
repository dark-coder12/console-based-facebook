#include <fstream>
#include <iostream>
#include <string.h>
#include <type_traits>

using namespace std;

class Date;
class Post;
class User;
class Page;
class Friend;
class Helper;
class Activity;
class Comments;
class AuthorName;
class SocialNetwork;

// Template Functions for Efficient Deallocation of Dynamic Arrays


template <typename D>
void OneD_Destructer(D*& OneD_Arr)
{
	if (OneD_Arr)
	{
		delete[] OneD_Arr;

		OneD_Arr = nullptr;
	}
}

template <typename D>
void TwoD_Destructer(D**& twoD_Arr, int size)
{
	if (twoD_Arr)
	{
		for (int i = 0; i < size; i++)
		{
			delete twoD_Arr[i];
		}
		delete[] twoD_Arr;

		twoD_Arr = nullptr;
	}
}



//____________________________________________________ Author Name Class ___________________________________________________________________________//

// AuthorName is a Base Class for Page and User Class with a purpose to save name of sharer of a post
// may it be a Page, or a User, and both of these classes are inherited with AuthorName

class AuthorName
{
public:

	virtual char* GetSharedBy()
	{
		return nullptr;
	}

	virtual void PrintAuthorID() {}

	virtual void PrintAuthorName() {}

	virtual void SetTimeline(Post*& posts) {}

	void PrintAuthorDetail()
	{
		cout << "\t";

		PrintAuthorID();

		cout << "  :  ";

		PrintAuthorName();

		cout << endl;
	}
};

//_________________________________________________________ Helper Class _______________________________________________________________________________//

// Helper Class contains manual functions to accomodate C Strings to find string length, allocate 
// memory, to check if two strings are equal, or to concatenate two given strings 

class Helper
{
public:

	static int StringLength(const char* str)
	{
		int strlength = 0;

		for (int i = 0; str[i] != '\0'; i++)
		{
			strlength++;
		}
		return strlength;
	}

	static char* GetStringFromBuffer(const char* buffer)
	{
		int strLen = StringLength(buffer);

		char* str = 0;

		if (strLen > 0)
		{
			str = new char[strLen + 1];

			char* deepCopy = str;

			for (const char* tempSrc = buffer; *tempSrc != '\0'; tempSrc++, deepCopy++)
			{
				*deepCopy = *tempSrc;
			}
			*deepCopy = '\0';
		}
		return str;
	}

	static bool SearchString(const char* str1, const char* str2)
	{
		int check = 0;

		for (int i = 0; i < StringLength(str1); i++)
		{
			if (str1[i] == str2[i])
			{
				check++;
			}
		}
		if (check == StringLength(str1))
		{
			return true;
		}
		else return false;
	}

	static char* StringConcatenate(char* str1, char* str2)
	{
		char* cat_String = 0;

		int index = 0, index_2 = 0;

		int cat_Len = StringLength(str1) + StringLength(str2) + 2;

		cat_String = new char[cat_Len];

		while (index < StringLength(str1))
		{
			cat_String[index] = str1[index];
			index++;

			if (index == (StringLength(str1) - 1))
			{
				cat_String[index + 1] = ' ';
			}
		}
		index++;

		while (index < cat_Len - 1)
		{
			cat_String[index] = str2[index_2];

			index++;

			index_2++;

			if (index == (cat_Len - 2))
			{
				cat_String[index + 1] = '\0';
			}
		}

		return cat_String;
	}
};

//___________________________________________________ Date Class _________________________________________________________________________________//

// A Date Class allows the User to Set the System date for Facebook and also sets the posted date for
// each post respectively, it also prints dates in correct format of (Date, Month, Year)

class Date
{ 
private:
	int day;
	int month;
	int year;

public:
	static Date SystemDate;

	Date(int d = 0, int m = 0, int y = 0)
	{
		day = d, month = m, year = y;
	}

	void PrintPostDate()
	{
		cout << "(" << day << " / " << month << " / " << year << ")";
	}

	bool RecentPostsChecker()
	{
		if (day == SystemDate.day - 1 || day == SystemDate.day)
		{
			if (month == SystemDate.month && year == SystemDate.year)          // timeline only includes recent posts
			{
				return true;
			}
		}
		return false;
	}
};

//____________________________________________________  Comment Class _______________________________________________________________________________//

// The Post Class is associated with the comment Class as each post can have a comment that can be either 
// written by a User or Page (Class AuthorName holds the name of Sharer). The Comment Class includes the
// text of the written comment, and its unique ID which is read from the file for example c1 or c2. It
// is also responsible for printing of the comment

class Comments
{
private:
	char* commentID;
	char* commentText;

	AuthorName* nameOfSharer;

public:

	static int totalComments;

	Comments()
	{
		commentID = nullptr;
		commentText = nullptr;
		nameOfSharer = nullptr;
	}

	~Comments()
	{
		OneD_Destructer(commentID);
		OneD_Destructer(commentText);
	}

	void SetAuthorName(AuthorName* name)
	{
		nameOfSharer = name;
	}

	const char* IDReturn()
	{
		return commentID;
	}

	void LoadID(ifstream& commentFile)
	{
		char tempBuffer[30];

		commentFile >> tempBuffer;
		commentID = Helper::GetStringFromBuffer(tempBuffer);
	}

	void LoadText(ifstream& commentFile)
	{
		char tempBuffer[100];

		commentFile.getline(tempBuffer, 100, '\n');
		commentText = Helper::GetStringFromBuffer(tempBuffer);
	}

	void PrintSingleComment()
	{
		cout << "                 ";

		nameOfSharer->PrintAuthorName();

		cout << " wrote : " << commentText << endl;

	}
	void SetNewComment(AuthorName* name, char* newText)
	{
		Comments::totalComments++;

		nameOfSharer = name;

		char tempCommentID[10] = "c", integerIDNum[10];

		_itoa_s(Comments::totalComments, integerIDNum, 10);

		integerIDNum[Helper::StringLength(integerIDNum)] = '\0';

		strcat_s(tempCommentID, integerIDNum);

		commentID = Helper::GetStringFromBuffer(tempCommentID);

		commentText = Helper::GetStringFromBuffer(newText);
	}

};


//_____________________________________________________ Post Class ________________________________________________________________________________//

// Post Class is responsble for storing each User's or Page's Posts and also the comments of that post A post 
// can be a simple Post of Type 1 or an Activity  of Type 2 thus the Activity Class inherits the Post Class.
// This Class keeps track of how many posts there are per user / page, and how many have liked that post.

class Post
{
private:
	char* ID;
	char* Text;

	Comments** commentsUnderPost;

protected:
	AuthorName* sharedBy;
	AuthorName** likedBy;

public:
	Date* postDate;

	int likedPerPost;
	int commentCounter;
	int totalCommentsOfPost;

	Post()
	{
		ID = nullptr;
		Text = nullptr;
		likedBy = nullptr;
		postDate = nullptr;
		sharedBy = nullptr;
		commentsUnderPost = nullptr;

		likedPerPost = 0;
		commentCounter = 0;
		totalCommentsOfPost = 0;
	}

	virtual ~Post()
	{
		OneD_Destructer(ID);
		OneD_Destructer(Text);
		OneD_Destructer(likedBy);
		TwoD_Destructer(commentsUnderPost, commentCounter);

	}

	AuthorName* GetSharedBy()
	{
		return sharedBy;
	}

	const char* IDReturn()
	{
		return ID;
	}

	char* TextReturn()
	{
		return Text;
	}

	void PrintText()
	{
		cout << Text;
	}
	void SetSharedBy(AuthorName* name)
	{
		sharedBy = name;
	}

	void SetComment(Comments*& comment)
	{
		if (commentsUnderPost == 0)
		{
			commentsUnderPost = new Comments * [10];
		}

		if (commentCounter < 10)                        // if the comments are under ten, a new Comment is made
		{
			commentsUnderPost[commentCounter] = comment;
			commentCounter++;
		}
	}

	void SetLikedBy(AuthorName* name, int& index)
	{
		if (likedBy == 0)
		{
			likedBy = new AuthorName * [10];            // total ten Users or Pages can like a post 
		}

		if (index < 10)
		{
			likedBy[index] = name;
		}
	}

	virtual void Print()                                  // view home utilizes this and shows date too
	{
		sharedBy->PrintAuthorName();

		cout << " shared \" " << Text << " \"              ";

		postDate->PrintPostDate();

		cout << endl;

		PrintAllComments();

		cout << "-----------------------------------------------------------------------------------------------" << endl << endl;

	}

	virtual void PrintWithoutDate()                       // timeline shows posts without date (recent only)
	{
		sharedBy->PrintAuthorName();

		cout << " shared \" " << Text << " \"  " << endl;

		PrintAllComments();

		cout << "-----------------------------------------------------------------------------------------------" << endl << endl;

	}

	void PrintAllComments()
	{
		if (commentCounter)
		{
			cout << "\n\n\t\t\t\tComment Box" << endl << endl;
		}

		for (int i = 0; i < commentCounter; i++)
		{
			commentsUnderPost[i]->PrintSingleComment();
		}
	}

	virtual void LoadSinglePostFromFile(ifstream& postFile)
	{
		char tempID[10], tempText[100];

		postFile.ignore();
		postFile.get(tempID, 10, '\n');

		ID = Helper::GetStringFromBuffer(tempID);

		int day, month, year;
		postFile >> day >> month >> year;

		postDate = new Date(day, month, year);

		postFile.ignore();
		postFile.getline(tempText, 100, '\n');

		Text = Helper::GetStringFromBuffer(tempText);
	}

	void NewLikedSetterFromPost(AuthorName* liker)
	{
		int checkIfLiked = 0;

		if (likedPerPost < 10)
		{
			for (int i = 0; i < likedPerPost; i++)
			{
				if (likedBy[i] == liker)
				{
					cout << "You have already liked this Post!";           // A User / Page can not like an already liked post
					checkIfLiked = 1;

				}
			}
			if (checkIfLiked == 0)                                          // this sets a like on the next available row
			{
				likedBy[likedPerPost] = liker;

				cout << "You have successfully liked ";

				likedBy[likedPerPost]->PrintAuthorName();

				cout << "!" << endl << endl;

				likedPerPost++;
			}
		}
	}

	void NewCommentSetterFromPost(char* Text, AuthorName* commenter)
	{
		if (commentCounter < 10)
		{
			commentsUnderPost[commentCounter] = new Comments;              // this sets a new comment with given text 
			commentsUnderPost[commentCounter]->SetNewComment(commenter, Text);
			commentCounter++;
		}
	}

	void ShowLikesOfPost()
	{
		for (int i = 0; i < likedPerPost; i++)
		{
			likedBy[i]->PrintAuthorDetail();
		}
	}

	virtual char* SearchCaption()
	{
		return Text;

	}
};

//____________________________________________________ Activity Class _____________________________________________________________________________//

// Activity is a type of Post, which is a little different than a simple post as it includes some
// already set activities such as celebrating, making, feeling, thinking about, and thus it includes
// a separate Print function. The Value of the Activity includes what the activity text is, and the 
// type includes a number from 1 to 4 which includes one of the activities respectively.

class Activity : public Post
{
private:
	int Type;
	char* Value;

public:

	Activity()
	{
		Type = 0;
		Value = nullptr;
	}
	~Activity()
	{
		OneD_Destructer(Value);
	}

	void PrintActivityType()
	{
		if (Type == 1) 	cout << " is feeling ";
		if (Type == 2)      cout << " is thinking about ";
		if (Type == 3)  	cout << " is making ";
		if (Type == 4)      cout << " is celebrating ";

	}
	void Print()
	{
		sharedBy->PrintAuthorName();

		PrintActivityType();

		cout << Value << "\n\n\n \" ";
		PrintText();
		cout << " \"\t\t\t\t";

		postDate->PrintPostDate();

		cout << endl << endl;

		PrintAllComments();

		cout << "-----------------------------------------------------------------------------------------------" << endl << endl;
	}

	void PrintWithoutDate()
	{
		sharedBy->PrintAuthorName();

		PrintActivityType();

		cout << Value << "\n\n\n \" ";

		PrintText();

		cout << endl << endl;

		PrintAllComments();

		cout << "-----------------------------------------------------------------------------------------------" << endl << endl;
	}

	void LoadSinglePostFromFile(ifstream& activityFile)
	{
		Post::LoadSinglePostFromFile(activityFile);

		activityFile >> Type;

		char tempValue[50];

		activityFile.ignore();
		activityFile.get(tempValue, 50, '\n');

		Value = Helper::GetStringFromBuffer(tempValue);
		activityFile.ignore();
	}

	char* SearchCaption()
	{
		return Value;

	}
};


//_______________________________________________________ Page Class ________________________________________________________________________________//

// The Page Class includes information of all of the Pages that are signed up on Facebook
// Each Page has its unique ID such as p1 or p3, and its tile, such as Dawn.com which is a 
// news paper Page. Each Page also has a timeline, which includes all of its own posts

class Page : public AuthorName
{
private:
	char* ID;
	char* Title;

	Post** timeline;

public:

	int postCounter;

	Page()
	{
		postCounter = 0;

		timeline = nullptr;
		ID = Title = nullptr;
	}

	~Page()
	{
		OneD_Destructer(ID);
		OneD_Destructer(Title);
		TwoD_Destructer(timeline, postCounter);
	} 

	char* GetSharedBy()
	{
		return ID;
	}

	char* Page_ID_Getter()
	{
		return ID;
	}

	void PrintAuthorName()
	{
		cout << Title << " ";
	}

	void DisplayPageDetail()
	{
		cout << ID << " " << Title << endl;
	}

	void PrintAuthorID()
	{
		cout << ID;
	}

	void PrintPageTitle()
	{
		cout << Title;
	}

	char* Page_Title_Getter()
	{
		return Title;
	}

	void SetTimeline(Post*& posts)                                 // sets the posts of that Page into its timeline
	{
		if (timeline == 0)
		{
			timeline = new Post * [10];
		}
		timeline[postCounter] = posts;
		postCounter++;
	}

	void ViewTimeline()
	{
		for (int i = 0; i < postCounter; i++)
		{
			timeline[i]->Print();
		}
	}

	void PrintLikedPagesPosts()
	{
		for (int i = 0; i < postCounter; i++)
		{
			if (timeline[i]->postDate->RecentPostsChecker())
			{
				timeline[i]->PrintWithoutDate();
			}
		}
	}

	void LoadPageInformation(ifstream& pagesList)
	{
		char tempID[50] = { 0 }, tempTitle[50] = { 0 };

		pagesList >> tempID;

		ID = Helper::GetStringFromBuffer(tempID);                          // ID is without spaces

		pagesList.ignore();

		pagesList.getline(tempTitle, 100, '\n');                         // Title can be multiple words

		Title = Helper::GetStringFromBuffer(tempTitle);
	}
};

//_____________________________________________________ User Class _____________________________________________________________________________//

// The User Class includes information of all of the Users that are signed up on Facebook
// Each User has its unique ID such as u1 or u7, and the persons first and last name. Each
// User also has a timeline, which includes all of its own posts.There is also a Home Page
//  of the  recent posts (in 24 Hours) of its liked pages, and friends.

class User : public AuthorName

{
private:
	char* ID;
	char* fName;
	char* lName;

	Post** timeline;
	Page** LikedPages;
	User** FriendList;


public:
	int postCounter;
	int friendCountUser;
	int likedPagesByUser;


	User()
	{
		ID = nullptr;

		FriendList = nullptr;
		LikedPages = nullptr;

		timeline = nullptr;
		fName = lName = nullptr;

		postCounter = 0;
		friendCountUser = 0;
		likedPagesByUser = 0;
	}

	~User()
	{
		OneD_Destructer(ID);
		OneD_Destructer(fName);
		OneD_Destructer(lName);
		OneD_Destructer(FriendList);
		OneD_Destructer(LikedPages);
		TwoD_Destructer(timeline, postCounter);
	}


	char* User_ID_Getter()
	{
		return ID;
	}

	char* User_FirstName_Getter()
	{
		return fName;
	}

	char* User_LastName_Getter()
	{
		return lName;
	}


	char* GetSharedBy()
	{
		return ID;
	}

	void PrintAuthorID()
	{
		cout << ID;
	}

	void SetFriends()
	{
		FriendList = new User * [10];
	}

	void SetLikedPages()
	{
		LikedPages = new Page * [10];
	}


	void SetTimeline(Post*& posts)                               // sets the posts of that User into timeline
	{
		if (timeline == 0)
		{
			timeline = new Post * [10];
		}
		timeline[postCounter] = posts;
		postCounter++;
	}

	void PrintAuthorName()
	{
		cout << fName << " " << lName << "  ";
	}

	void ViewTimeline()
	{
		for (int i = 0; i < postCounter; i++)
		{
			timeline[i]->Print();
		}
	}

	void LoadUserInformation(istream& singleUser)
	{
		char userID[10];

		char tempFirstName[30], tempLastName[30];

		singleUser >> userID;

		ID = Helper::GetStringFromBuffer(userID);

		singleUser >> tempFirstName >> tempLastName;

		fName = Helper::GetStringFromBuffer(tempFirstName);

		lName = Helper::GetStringFromBuffer(tempLastName);
	}

	void ViewPages()
	{
		for (int i = 0; i < likedPagesByUser; i++)
		{
			cout << "Page   " << i + 1 << ":    ";

			LikedPages[i]->DisplayPageDetail();
		}
	}

	void ViewFriends()
	{
		for (int i = 0; i < friendCountUser; i++)
		{
			cout << "Friend " << i + 1 << ":    " << FriendList[i]->ID;

			if (Helper::StringLength(FriendList[i]->ID) > 2)
			{
				cout << "        ";
			}
			else
				cout << "         ";

			cout << FriendList[i]->fName << "   " << FriendList[i]->lName << endl;
		}
		cout << endl;
	}

	void AddPages(int& index, Page* SearchPagesList)
	{

		if (likedPagesByUser < 10)
		{
			LikedPages[index] = SearchPagesList;
		}
	}

	void AddFriends(int& index, User* SearchFriendsList)
	{
		if (friendCountUser < 10)
		{
			FriendList[index] = SearchFriendsList;
		}
	}

	void PrintLatestFriendPosts()
	{
		for (int i = 0; i < postCounter; i++)
		{
			if (timeline[i]->postDate->RecentPostsChecker())         // if a post is recent, it is printed
			{
				timeline[i]->PrintWithoutDate();
			}
		}
	}

	void ViewHome()
	{
		for (int i = 0; i < friendCountUser; i++)
		{
			FriendList[i]->PrintLatestFriendPosts();

		}

		for (int i = 0; i < likedPagesByUser; i++)
		{
			LikedPages[i]->PrintLikedPagesPosts();
		}
	}
};


//_________________________________________________ Social Network Class _____________________________________________________________________//

// The Social Network Class is the main Class holding all of the information of all Users
// and Pages by file reading and then connects the information through Association with 
// double pointers. It also includes all of the posts of the pages and users, along with its 
// comment list. It allows a User / Page to like a post or to comment on it.

class SocialNetwork
{
private:
	Page** PageList;
	User** UsersList;
	Post** PostsList;
	User* CurrentUser;

public:

	int totalPostLikes;

	static int totalPages;
	static int totalUsers;
	static int totalPosts;
	static int totalComments;

	SocialNetwork()
	{
		PageList = nullptr;
		UsersList = nullptr;
		PostsList = nullptr;
		CurrentUser = nullptr;
		totalPostLikes = 0;
	}

	~SocialNetwork()
	{
		OneD_Destructer(PostsList);

		TwoD_Destructer(PageList, totalPages);
		TwoD_Destructer(UsersList, totalUsers);
	}

	User* SearchUserID(const char* str1)
	{
		for (int i = 0; i < SocialNetwork::totalUsers; i++)
		{
			const char* str2 = UsersList[i]->User_ID_Getter();

			if (Helper::SearchString(str1, str2) == true)                      // if a User is found it returns the pointer
			{
				return UsersList[i];
			}
		}
		return nullptr;
	}

	Page* SearchPageID(const char* str1)
	{
		for (int i = 0; i < totalPages; i++)
		{
			char* str2 = PageList[i]->Page_ID_Getter();

			if (Helper::SearchString(str1, str2) == true)                   // if a Page is found it returns the pointer
			{
				return PageList[i];
			}
		}
		return nullptr;
	}

	Post* SearchPostID(const char* str1)
	{
		for (int i = 0; i < totalPosts; i++)
		{
			const char* str2 = PostsList[i]->IDReturn();

			if (Helper::SearchString(str1, str2) == true)				// if a Post is found it returns the pointer
			{
				return PostsList[i];
			}
		}
		return nullptr;
	}

	AuthorName* UserPageChecker(char* tempSharedBy)
	{
		if (tempSharedBy[0] == 'u')
		{
			return (SearchUserID(tempSharedBy));                             // if found an author points to User
		}
		else
		{
			return (SearchPageID(tempSharedBy));                            // if found an author points to Page
		}
	}

	void AssociateFriends(char***& tempFriendList)
	{
		for (int i = 0; i < SocialNetwork::totalUsers; i++)
		{
			UsersList[i]->SetFriends();

			for (int j = 0; j < UsersList[i]->friendCountUser; j++)
			{
				if (SearchUserID(tempFriendList[i][j]))
				{
					UsersList[i]->AddFriends(j, SearchUserID(tempFriendList[i][j]));          // saves friend of user if found
				}

			}
		}

		for (int i = 0; i < totalUsers; i++)
		{
			for (int j = 0; j < FriendCountReturner(i); j++)
			{
				delete tempFriendList[i][j];                          // 3D Array Deallocation

				tempFriendList[i][j] = nullptr;
			}
		}
		delete[] tempFriendList;
	}

	void AssociatePages(char***& tempPagesList)
	{
		for (int i = 0; i < SocialNetwork::totalUsers; i++)
		{
			UsersList[i]->SetLikedPages();

			for (int j = 0; j < UsersList[i]->likedPagesByUser; j++)
			{
				if (SearchPageID(tempPagesList[i][j]))                 // association with pages
				{
					UsersList[i]->AddPages(j, SearchPageID(tempPagesList[i][j]));
				}
			}
		}
		for (int i = 0; i < totalUsers; i++)
		{
			for (int j = 0; j < likedPageReturner(i); j++)             // deletes all 3 dimensions
			{
				delete [] tempPagesList[i][j];

				tempPagesList[i][j] = nullptr;
			}

			delete [] tempPagesList[i];
		}
		delete[] tempPagesList;
	}

	int likedPageReturner(int index)
	{
		return UsersList[index]->likedPagesByUser;
	}

	int FriendCountReturner(int index)
	{
		return UsersList[index]->friendCountUser;
	}

	void LoadingAllUsersInformation(ifstream& usersList, ifstream& pagesList, char***& tempFriendList, char***& tempLikedPages)
	{
		usersList >> totalUsers;

		UsersList = new User * [totalUsers];

		tempFriendList = new char** [totalUsers];

		tempLikedPages = new char** [totalUsers];

		char tempFriendBuffer[50][50], tempPageBuffer[50][50];

		for (int i = 0; i < totalUsers; i++)
		{
			UsersList[i] = new User;

			UsersList[i]->LoadUserInformation(usersList);

			int pageCount = 0, friendCount = 0, negativeOneChecker = 0;

			for (int f = 0; negativeOneChecker == 0; f++)
			{
				usersList >> tempFriendBuffer[f];

				if (tempFriendBuffer[f][0] == '-' && tempFriendBuffer[f][1] == '1')            // saves data till delimiter -1
				{
					negativeOneChecker = 1;
				}
				else
				{
					friendCount++;
				}
			}
			for (int p = 0; negativeOneChecker == 1; p++)
			{
				usersList >> tempPageBuffer[p];

				if (tempPageBuffer[p][0] == '-' && tempPageBuffer[p][1] == '1')
				{
					negativeOneChecker = 0;
				}
				else
				{
					pageCount++;
				}
			}

			UsersList[i]->friendCountUser = friendCount;

			tempFriendList[i] = new char* [friendCount];

			for (int f = 0; f < friendCount; f++)
			{
				tempFriendList[i][f] = Helper::GetStringFromBuffer(tempFriendBuffer[f]);
			}

			UsersList[i]->likedPagesByUser = pageCount;

			tempLikedPages[i] = new char* [pageCount];

			for (int p = 0; p < pageCount; p++)
			{
				tempLikedPages[i][p] = Helper::GetStringFromBuffer(tempPageBuffer[p]);
			}

		}
		pagesList >> totalPages;

		PageList = new Page * [totalPages];

		for (int i = 0; i < totalPages; i++)
		{
			PageList[i] = new Page;

			PageList[i]->LoadPageInformation(pagesList);
		}
		usersList.close();

		pagesList.close();

	}

	void LoadAllPosts(ifstream& AllPosts)
	{
		int type = 0;

		AllPosts >> totalPosts;

		PostsList = new Post * [totalPosts];

		for (int i = 0; i < totalPosts; i++)
		{
			AllPosts.ignore();

			AllPosts >> type;

			if (type == 1)
			{
				PostsList[i] = new Post;                              // depending on the type a post is created


			}
			if (type == 2)
			{
				PostsList[i] = new Activity;                          // depending on the type an activity is created

			}

			PostsList[i]->LoadSinglePostFromFile(AllPosts);

			char tempSharedBy[5];

			AllPosts.get(tempSharedBy, 5, '\n');

			PostsList[i]->SetSharedBy(UserPageChecker(tempSharedBy));

			char tempLikedByList[10][10] = { 0 };

			int negativeOneChecker = 0;

			for (int b = 0; negativeOneChecker == 0; b++)
			{
				AllPosts >> tempLikedByList[b];

				if (tempLikedByList[b][0] == '-')
				{
					negativeOneChecker = 1;
				}
				else
				{
					totalPostLikes++;
				}
			}

			for (int a = 0; a < totalPostLikes; a++)
			{
				PostsList[i]->SetLikedBy(UserPageChecker(tempLikedByList[a]), a);       // if user id is found, it is set to liker
			}

			PostsList[i]->GetSharedBy()->SetTimeline(PostsList[i]);

			PostsList[i]->likedPerPost = totalPostLikes;

			totalPostLikes = 0;
		}
		AllPosts.close();
	}

	void LoadAllComments(ifstream& commentFile)
	{
		char tempPostedBy[10], author[10];

		commentFile >> totalComments;

		Comments::totalComments = totalComments;

		Comments** CommentsList = new Comments * [totalComments];

		for (int i = 0; i < totalComments; i++)
		{
			CommentsList[i] = new Comments;

			CommentsList[i]->LoadID(commentFile);

			commentFile.ignore();

			commentFile >> tempPostedBy;

			SearchPostID(tempPostedBy)->SetComment(CommentsList[i]);                             // association with posts

			commentFile >> author;

			CommentsList[i]->SetAuthorName(UserPageChecker(author));

			commentFile.ignore();

			CommentsList[i]->LoadText(commentFile);
		}
		commentFile.close();

		OneD_Destructer(CommentsList);
	}

	void SetCurrentUser(const char* setUser)
	{
		CurrentUser = SearchUserID(setUser);
	}

	void CommentOnAPost(Post* postToComment, User* commenter, char* commentBuff)             // A User can comment on a post
	{
		postToComment->NewCommentSetterFromPost(commentBuff, commenter);

	}

	void LikeAPost(Post*& postToLike, User* liker)
	{
		if (postToLike->likedPerPost < 10)
		{
			postToLike->NewLikedSetterFromPost(liker);                                 // A User can like a page under 10 likes
		}
	}


	template <typename T1, typename T2, typename T3>

	void ReturnSearch(T1 classToSearch, T2 size, T3 keyword)          // a template that allows to search from Users, Posts, Pages
	{
		int totalFound = 0;

		char* word = 0;

		for (int i = 0; i < size; i++)
		{
			if (typeid(T1) == typeid(UsersList))
			{
				word = Helper::StringConcatenate(UsersList[i]->User_FirstName_Getter(), UsersList[i]->User_LastName_Getter());
			}
			if (typeid(T1) == typeid(PageList))
			{
				word = PageList[i]->Page_Title_Getter();
			}

			if (typeid(T1) == typeid(PostsList))
			{
				word = PostsList[i]->SearchCaption();
			}

			for (int j = 0; j < Helper::StringLength(word); j++)
			{
				int name_Exist = 0;

				char first_Letter = word[j];

				if ((char(keyword[0]) == char(first_Letter) || (char(keyword[0] + 32) == char(first_Letter) || (char(keyword[0] - 32) == char(first_Letter)))))
				{

					int startIndex_Row = i;
					int startIndex_Col = j + 1;

					for (int k = 1; k < Helper::StringLength(keyword); k++)
					{						                                             // allows Case Sensitivity for efficiency

						if (keyword[k] != (word[startIndex_Col]) && char(keyword[k] + 32) != (word[startIndex_Col] && char(keyword[k] - 32) != (word[startIndex_Col])))
						{
							name_Exist = 1;
						}
						startIndex_Col++;
					}

					if (name_Exist == 0)
					{
						if (totalFound == 0)
						{
							if (typeid(T1) == typeid(UsersList))
							{
								cout << "\n\t\t\t The Users Found are as follows :" << endl << endl;
							}
							if (typeid(T1) == typeid(PageList))                             // different functionality for different types
							{
								cout << "\n\t\t\t The Pages Found are as follows :" << endl << endl;
							}
							if (typeid(T1) == typeid(PostsList))
							{
								cout << "\n\t\t\t The Posts Found are as follows :" << endl << endl;
							}
						}
						if (typeid(T1) == typeid(UsersList))
						{
							UsersList[i]->PrintAuthorDetail();
						}
						if (typeid(T1) == typeid(PageList))
						{
							PageList[i]->PrintAuthorDetail();
						}
						if (typeid(T1) == typeid(PostsList))
						{
							PostsList[i]->Print();

						}

						totalFound++;
					}
				}
			}
		}
	}

	void Run()                                                            // the required output is hardcoded in Run Function
	{

		cout << "_______________________________________________________________________________________________" << endl << endl;

		cout << "                                            Welcome to FaceBook                               " << endl << endl;

		char userSearch[4] = "u7";

		cout << "\t\t   Social Networking - Facebook Navigation Menu will give the Following Options " << endl << endl;


		cout << " 0. Quit Facebook Menu. \n 1. View Home. \n 2. Like a Post. \n 3. View Liked List of a Post. \n 4. Comment on a Post. \n 5. View a Post. \n 6. View User's Timeline. \n 7. View FriendList. \n 8. View Liked Pages. \n 9.View TimeLine of a Page. \n 10. Search All Users, Pages & Posts. " << endl << endl;


		cout << "_______________________________________________________________________________________________" << endl << endl;


		SetCurrentUser(userSearch);


		cout << "                                   Command : Set User to - " << userSearch << endl << endl;

		cout << "                        Current User is successfully set to  : " << userSearch << " - ";

		CurrentUser->PrintAuthorName();

		cout << endl << endl << "_______________________________________________________________________________________________" << endl << endl;


		cout << "                        Command : Set Current System Date to:  15 11 2017 " << endl << endl;

		cout << "                                   System Date : "; Date::SystemDate.PrintPostDate(); cout << endl << endl;

		cout << "_______________________________________________________________________________________________" << endl << endl;

		if (CurrentUser)
		{

			cout << "                        Command : View Friend List of Current User" << endl << endl;

			cout << "                    Viewing Friend List of ";

			CurrentUser->PrintAuthorName();

			cout << endl << endl;

			CurrentUser->ViewFriends();
		}

		else	cout << "Invalid User ID - User ID does not exist." << endl << endl;

		cout << "_______________________________________________________________________________________________" << endl << endl;


		cout << "                        Command : View Liked Pages List of Current User" << endl << endl;

		if (CurrentUser)
		{

			cout << "                  Viewing Liked Pages List of ";

			CurrentUser->PrintAuthorName();

			cout << endl << endl;

			CurrentUser->ViewPages();
		}

		else cout << "Invalid User ID - User ID does not exist." << endl << endl;



		cout << "_______________________________________________________________________________________________" << endl << endl;


		cout << "                                Command : View Home of  ";

		CurrentUser->PrintAuthorName();

		cout << endl << endl;

		if (CurrentUser)
		{
			CurrentUser->ViewHome();

			cout << " You're all caught up! Showing all posts of the past 24 hours! " << endl << endl;

		}

		else cout << "Invalid User ID - User ID does not exist." << endl << endl;


		cout << "_______________________________________________________________________________________________" << endl << endl;


		cout << "                         Command : View TimeLine of  ";

		CurrentUser->PrintAuthorName();

		if (CurrentUser)
		{
			cout << "\n\n\t\t\t Now Viewing Timeline of ";

			CurrentUser->PrintAuthorName();

			cout << endl << endl;

			CurrentUser->ViewTimeline();
		}

		else cout << "Invalid User ID - User ID does not exist." << endl << endl;

		cout << "_______________________________________________________________________________________________" << endl << endl;


		char PostID[10] = "post5";

		cout << "                                    Command : View Liked List of Post  " << PostID << endl << endl;


		Post* viewPostLikes = SearchPostID(PostID);

		if (viewPostLikes)
		{
			cout << endl << endl << "All Likes of " << PostID << endl << endl;

			viewPostLikes->ShowLikesOfPost();
		}

		else cout << "Invalid Post ID - Post ID does not exist." << endl << endl;

		cout << "_______________________________________________________________________________________________" << endl << endl;


		char likePostID[10] = "post5";

		cout << "                                    Command : Liking Post of " << likePostID << endl << endl;

		cout << "                             Command : View Liked List of Post  " << likePostID << endl << endl;

		Post* postToLike = SearchPostID(likePostID);

		if (postToLike)
		{
			LikeAPost(postToLike, CurrentUser);

			postToLike->ShowLikesOfPost();
		}

		else cout << "Invalid Post ID - Post ID does not exist." << endl << endl;

		cout << "_______________________________________________________________________________________________" << endl << endl;

		char commentPostID[10] = "post4";
		 
		cout << "                             Command : Write a comment on Post " << commentPostID << endl << endl;

		cout << "                                    Command : View Post " << commentPostID << endl << endl;

		Post* postToComment = SearchPostID(commentPostID);

		if (postToComment)
		{
			char commentBuff[50] = "Good Luck for your result";

			CommentOnAPost(postToComment, CurrentUser, commentBuff);

			cout << endl << endl << "Your comment has been successfully added! " << endl << endl;

			postToComment->Print();
		}
		else	cout << "Invalid Post ID - Post ID does not exist." << endl << endl;

		cout << "_______________________________________________________________________________________________" << endl << endl;

		char commentPostID2[10] = "post8";

		cout << "                             Command : Write a comment on Post " << commentPostID2 << endl << endl;

		cout << "                                    Command : View Post " << commentPostID2 << endl << endl;

		Post* postToComment2 = SearchPostID(commentPostID2);

		if (postToComment2)
		{
			char commentBuff[50] = "Thanks for the wishes";

			CommentOnAPost(postToComment2, CurrentUser, commentBuff);

			cout << endl << endl << "Your comment has been successfully added! " << endl << endl;

			postToComment2->Print();
		}
		else	cout << "Invalid Post ID - Post ID does not exist." << endl << endl;

		cout << "_______________________________________________________________________________________________" << endl << endl;

		char displayPageID[4] = "p1";

		Page* displayPage = SearchPageID(displayPageID);

		cout << "                         Command : View Page ";

		displayPage->PrintAuthorID();

		cout << "\n\n\t\t\t Now Viewing Timeline of ";

		displayPage->PrintPageTitle();

		cout << "!" << endl << endl;

		if (displayPage)
		{
			displayPage->ViewTimeline();
		}

		else	cout << "Invalid Page ID - Page ID does not exist." << endl << endl;

		char  keyword_1[] = "Ali";

		cout << "_______________________________________________________________________________________________" << endl << endl;

		cout << "                             Command : Search for " << keyword_1 << endl << endl;

		ReturnSearch(UsersList, totalUsers, keyword_1);

		ReturnSearch(PostsList, totalPosts, keyword_1);

		ReturnSearch(PageList, totalPages, keyword_1);


		char keyword_2[] = "Birthday";

		cout << "_______________________________________________________________________________________________" << endl << endl;

		cout << "                              Command : Search for " << keyword_2 << endl << endl;

		ReturnSearch(UsersList, totalUsers, keyword_2);

		ReturnSearch(PostsList, totalPosts, keyword_2);

		ReturnSearch(PageList, totalPages, keyword_2);

		cout << "_______________________________________________________________________________________________" << endl << endl;

		cout << "                      You are now exiting FaceBook's Main Menu" << endl << endl;

		cout << "_______________________________________________________________________________________________" << endl << endl;
	}




};


//_________________________________  Static Member Intitalization of Social Network and Date Classes ___________________________________ // 

Date Date::SystemDate(15, 11, 2017);

int Comments::totalComments = 0;
int SocialNetwork::totalPages = 0;
int SocialNetwork::totalPosts = 0;
int SocialNetwork::totalUsers = 0;
int SocialNetwork::totalComments = 0;


//_______________________________________________ Main Function to Compile and Run Functions ____________________________________________ // 

void main()
{
	system("Color F0");                                                            // changes background color to Facebook's default color

	char*** tempFriendsList = nullptr;
	char*** likedPagesList = nullptr;

	ifstream pagesFile("SocialNetworkPages.txt");
	ifstream usersFile("SocialNetworkUsers.txt");
	ifstream postsFile("SocialNetworkPosts.txt");
	ifstream commentFile("SocialNetworkComments.txt");

	if (usersFile.is_open() && pagesFile.is_open() && postsFile.is_open() && commentFile.is_open())
	{
		{
			SocialNetwork FaceBook;

			FaceBook.LoadingAllUsersInformation(usersFile, pagesFile, tempFriendsList, likedPagesList);

			FaceBook.AssociateFriends(tempFriendsList);

			FaceBook.AssociatePages(likedPagesList);

			FaceBook.LoadAllPosts(postsFile);

			FaceBook.LoadAllComments(commentFile);

			FaceBook.Run();
		}
	}
	else
	{
		cout << "The Files Specified could not be opened successfully" << endl << endl;
	}

	system("pause");
}

