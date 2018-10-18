package com.medistro.util;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;


import javax.sql.DataSource;

public class dbConnection {
	private Connection connection = null;
	
	private String dataSourceName = "TryncPoolDs";

	//super
	public dbConnection() throws SQLException, ClassNotFoundException{
			Class.forName("com.mysql.jdbc.Driver");
			connection = DriverManager.getConnection("jdbc:mysql://localhost:3306/medistro","medistro", "medistro");
			connection.setAutoCommit(true); //default auto commit true
	}
	
	//commit
	public void commit(){
		try {
			connection.commit();
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}
	
	//close
	public void close() throws SQLException{
		//System.out.println("Closing database connection from datasource");
		connection.close();
	}
	
	//autocommit
	public void setAutoCommit(boolean autocommit){
		try {
			connection.setAutoCommit(autocommit);
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}
	
	//get connection
	public Connection getConnection(){
		return connection;
	}
}
