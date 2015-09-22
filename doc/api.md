# Client-server communication protocol

*****

### **getRooms**

**Description:** this method returns an array of information about the rooms.

**Route:** */api/get_rooms* [GET]

**Method signature:** response getRooms(void)

**Parameters:** There are no parameters

**Output:** JSON object. Example:

	{"response": 
		[
			{
				"id": "1",
				"name": "room1",
				"numPeople": "2",
				"maxNumPeople": "10"
			}
		]
	}

*****