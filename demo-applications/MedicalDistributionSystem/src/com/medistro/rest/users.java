package com.medistro.rest;

import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

import javax.ws.rs.ApplicationPath;
import javax.ws.rs.Consumes;
import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Application;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;

import org.codehaus.jettison.json.JSONException;
import org.codehaus.jettison.json.JSONObject;

import com.medistro.util.dbConnection;

@Path("/user")
@ApplicationPath("/rest")
public class users extends Application {

	// user login
	@POST
	@Consumes(MediaType.APPLICATION_JSON)
	@Produces(MediaType.APPLICATION_JSON)
	@Path("login")
	public Response login(String jsonLoginData) throws JSONException, ClassNotFoundException, SQLException {
		JSONObject userCredential = new JSONObject(jsonLoginData);
		Response response = null;

		String userSelectSQL = "SELECT userid,username,userlevel FROM users WHERE username=? and userpass=?";
		dbConnection con = new dbConnection();
		PreparedStatement pstmt = con.getConnection().prepareStatement(userSelectSQL);
		pstmt.setString(1, userCredential.getString("username"));
		pstmt.setString(2, userCredential.getString("password"));
		ResultSet rset = pstmt.executeQuery();
		if (rset.wasNull()) {// no matching users
			JSONObject errObject = new JSONObject();
			errObject.put("error", "Invalid creentials");
			response = Response.status(401).entity(errObject.toString()).build();
		} else {//matched one found
			rset.first();
			JSONObject userObject = new JSONObject();
			userObject.put("userid", rset.getInt("userid"));
			userObject.put("username", rset.getString("username"));
			userObject.put("userlevel", rset.getString("userlevel"));
			response = Response.ok().entity(userObject.toString()).build();
		}
		rset.close();
		pstmt.close();
		con.close();

		return response;
	}

}
