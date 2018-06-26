# Webserver design doc
### // TODO: Team Name

For project 9, we added two features to our webserver: markdown-to-HTML rendering
and a MySQL server interface.

---

## Feature #1: Markdown rendering

We used [cpp-markdown](https://sourceforge.net/projects/cpp-markdown/) to
convert Markdown files to HTML because it was the easiest to understand and 
was written in the same style as the rest of our project (C++ / boost).
To integrate Markdown rendering into our static file handler, all we had to do
was create a `markdown::Document` document for any files ending with the file
extension `.md`, read our file content into the doc, then get the resulting
HTML string. We additionally updated our static handler unit tests to ensure
that the handler would output the correct HTML when provided a simple markdown
file.

[Sample markdown file served by our server](http://54.190.63.110:8080/static1/markdown.md)

The markdown equivalent of that file can be found
[here](https://github.com/UCLA-CS130/TODO-Team-Name/blob/master/deploy/www/markdown.md)
(Github automatically renders `.md` files to HTML, but you can select 'raw' to see
what the raw file looks like).

This design document is an example of a Markdown file that has been converted by our
webserver to HTML!

---

## Feature #2: MySQL interface

Our second feature is a simple web interface to a MySQL server.

[The SQL interface is available here](http://54.190.63.110:8080/sql)

The page has two text inputs that can be used to query and update the database. We ran a
MySQL server in a container on our AWS instance, then use a c++ SQL client to connect to
the database and issue queries.

The SQL interface page list a few commands that you can execute to walk you through
some of the main features of the SQL interface. They are reproduced below:

* First, execute the following statement into the **Query** input to see a list of available tables:
  `SHOW Tables;`
* Let's take a closer look at TestTable! Enter the following into the **Query** input:
  `SELECT * FROM TestTable;`
* Now try adding an entry to the table. Enter the following into the **Update** input:
  `INSERT INTO TestTable VALUES ("5", "Shaghayegh");`
* Query the database to see the updated table using the **Query** input:
  `SELECT * FROM TestTable;`
* Let's say we want to change an entry. You can execute something like the following in the **Update** input:
  `UPDATE TestTable SET name="Valeed" WHERE id=5;`
* View your changes using the **Query** input:
  `SELECT * FROM TestTable;`
* Now let's delete that last entry using **Update**:
  `DELETE FROM TestTable WHERE id=5;`


### Future changes / modifications
Due to time constraints, this database interface is just a preliminary version.
The following features could be added to make it more user-friendly in the future:

* Asynchronously update the page when a query is receieved, instead of having to refresh the entire page
* Combine the query and update inputs into a single input so it's less confusing to the user
* Display the new contents of the table when it's updated, instead of just an 'Update Success' message




