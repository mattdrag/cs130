//
// sql_engine.cpp
// ~~~~~~~~~~~~~~~~~~~
//

#include <iostream>
#include "sql_engine.hpp"
#include <stdlib.h>
#include <string>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <resultset_metadata.h>
 
std::string SqlEngine::HandleRequest(std::string& field, int mode) {
  std::string out_string = "";

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;

    // Create a connection.
    driver = get_driver_instance();
    con = driver->connect("54.190.63.110:6603", "root", "");
    // Connect to the MySQL test database.
    con->setSchema("DB");

    // If it's a query, we want to return some results
    std::cout << "QueryStatement: " << field << "\n"; 
    stmt = con->createStatement();
    if (mode == 1){   // query mode
      res = stmt->executeQuery(field);
      // Get result set metadata
      sql::ResultSetMetaData *res_meta = res -> getMetaData();
      int columns = res_meta -> getColumnCount();

      out_string += "<table>";

      // Column attributes
      out_string += "<tr>";
      for (int i = 1; i <= columns; i++) {
        out_string += "<th>" + res_meta->getColumnName(i) + "</th>"; 
      }
      out_string += "</tr>";

      // Loop for each row.
      while (res->next()) {
        out_string += "<tr>";
        // Access column data by index, 1-indexed
        for (int i = 1; i <= columns; i++) {
          out_string += "<td>" + res->getString(i) + "</td>";
        }
        out_string += "</tr>";
      }

      out_string += "</table>";
      delete res;
    }
    // If it's an update, we don't need to return any results.
    else if (mode == 2){  // update mode
      stmt->execute(field);
      out_string = "Update Success!";
    }

    delete stmt;
    delete con;

  }
  catch (sql::SQLException &e) {
    out_string = "MYSQL ERR: INVALID SYNTAX";
  }

  return out_string;
}