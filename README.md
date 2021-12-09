# cppDB
## A database management system with a simple UI.
New features:
* **Import and export table contents** using .csv files
* **Index tables** by adding a primary key to a table

## How it works:
The UI is split into three tabs:
### **DBs** - List of all databases created using this program.
Here you can create, delete or select a database.

<img src=https://user-images.githubusercontent.com/77592590/145303633-fca994e0-8cda-4426-b1de-ecb29c53ffd0.png height=50% width=50%/>

**Deleting a database:**<br/>
In order to delete a database it first has to be empty - all tables must have been previously deleted from it.<br/>
Then, select the database by clicking its name from the list, and press the Delete DB button.<br/>

**Selecting a database to work with:**<br/>
To open a database to view its contents and alter it, simply click on its name from the list and press the Select DB button.

### **DB View** - List of tables from selected database
After selecting a database, the DB View tab will be automatically opened.<br/>
Here you can create, delete or select a table.

<img src=https://user-images.githubusercontent.com/77592590/145307377-83d5f680-1e9b-4590-a085-33b3dce42f88.png height=50% width=50%/>

**Deleting a table:**<br/>
In order to delete a table, you have to select it by clicking its name from the tables list and press on the Delete Table button.

**Selecting a table:**<br/>
Selecting a table works just like selecting a database. Click on its name and press the Select Table button.

**Creating a new table:**<br/>
Creating a new table can be done in two ways:

1. by providing a .csv file containing the name of the columns optionally followed by a list of entries

    - Sample .csv file for a table containing two columns and initialized with three entries:<br/><img src=https://user-images.githubusercontent.com/77592590/145308040-47ea4595-759b-4640-890c-402233da0404.png height=50% width=50%/>

    - Column data types must be added manually. Maximum number of characters must also be specified for strings.<br/><img src=https://user-images.githubusercontent.com/77592590/145308272-eb9dd5ea-723d-46ab-82fd-aaaef5984556.png height=50% width=50%/>

2. by manually adding columns to the table

    - Adding a column that is also the primary key of the table:<br/><img src=https://user-images.githubusercontent.com/77592590/145309245-f48b0a24-b0ed-4cd4-a2bb-6bcba80c52df.png width=50% heigth=50%/>

    - Adding a column that is not the primary key of the table.<br/>Only one column can be the primary key of the table.<br/><img src=https://user-images.githubusercontent.com/77592590/145309377-8be8014b-6c39-4bbc-b035-762609190623.png height=50% width=50%/>

After creating these two tables and the associative table SongRef<br/>that is used to add songs to playlists, the table list should look like this:<br/>
<img src=https://user-images.githubusercontent.com/77592590/145309538-350690c4-4ba8-4c4e-ab6c-8f8b7ffd46e3.png height=50% width=50%/>

### **Table View** - A canvas for viewing table description and search query results.
After selecting a table, the Table View tab will be automatically opened.<br/>
Rows can be inserted, selected, updated, and deleted from this tab.

**Viewing table description:**<br/>
Viewing the description of the current table can be done by pressing the View Table Data button.<br/>
This will populate the canvas with a list of the columns added to the table.<br/>
Each column has the following descriptors:
- A boolean value, wether it is a primary key or not
- A string, the name of the column
- The data type of the column
- The size in bytes occupied by the column on disk

**Inserting rows from a .csv file:**<br/>
Press the Insert Rows from .csv File button. This will open a dialogue box asking for a file.
Select a file and press Open. The program will automatically insert all valid rows from the file.

Sample .csv file containing 6 rows<br/><img src=https://user-images.githubusercontent.com/77592590/145310749-7bbb45ae-6ea4-4fcb-8c31-34294a3b3fbf.png height=50% width=50%/>

**Inserting rows manually:**<br/>
Pressing the Insert button will open a dialogue box asking for a new row to be inserted.<br/>
Each column value should be separated by whitespace. Strings sould not contain any whitespace characters.<br/><br/><img src=https://user-images.githubusercontent.com/77592590/145311032-e6742ec6-f174-450f-89b4-c919a89fd506.png height=50% width=50%/>

**Selecting rows from table:**<br/>
In order to show all rows that satisfy a \<column_name\> = \<value\> condition, press the Select button.<br/>
This will open a dialogue box asking for \<column_name\> and \<value\>.<br/>
After filling the fields and pressing the Ok button, the canvas will be populated with the selected rows.<br/><br/><img src=https://user-images.githubusercontent.com/77592590/145311410-f0662267-1ae4-4b66-91d2-82414e46d3af.png heigth=50% width=50%/>

**Updating rows from table:**<br/>
If you want to update a column for certain rows of the table, press the Update button.<br/>
After filling the required fields, press the Ok button and the table will be updated.<br/>
**Warning:** Updating the primary key may be dangerous.<br/>
If the new value already exists, the entire updated row will be deleted.<br/>
This is because there can be no duplicates in the primary key column<br/><br/><img src=https://user-images.githubusercontent.com/77592590/145311792-f8c560bf-8720-4442-8b39-0a6447c1cdfc.png height=50% width=50%/>

**Deleting rows from the table:**<br/>
Deleting rows works similarly to selecting them. The program asks for a \<column_name\> - \<value\> pair.<br/>
All rows that satisfy the \<column_name\> = \<value\> condition will be deleted.<br/><br/><img src=https://user-images.githubusercontent.com/77592590/145312246-c36fd28b-1bb7-489e-a39a-697c3737fa6f.png height=50% width=50%/>


