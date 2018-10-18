package com.medistro.rest;

import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

import javax.ws.rs.ApplicationPath;
import javax.ws.rs.Consumes;
import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.PUT;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Application;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;

import org.codehaus.jettison.json.JSONArray;
import org.codehaus.jettison.json.JSONException;
import org.codehaus.jettison.json.JSONObject;

import com.medistro.util.dbConnection;

@Path("/warehouse")
@ApplicationPath("/rest")
public class warehouse extends Application {

	// create
	@PUT
	@Consumes(MediaType.APPLICATION_JSON)
	@Produces(MediaType.APPLICATION_JSON)
	@Path("")
	public Response create(String jsonWhData) throws JSONException, ClassNotFoundException, SQLException {
		JSONObject whObject = new JSONObject(jsonWhData);
		Response response = null;
		int whid = 0;

		String insertWHSQL = "INSERT INTO warehouses (warehouse,whtype,medlonode) VALUES (?,?,?)";
		dbConnection con = new dbConnection();
		PreparedStatement pstmt = con.getConnection().prepareStatement(insertWHSQL);
		pstmt.setString(1, whObject.getString("warehouse"));
		pstmt.setString(2, whObject.getString("type"));
		pstmt.setString(3, whObject.getString("medlonode"));
		pstmt.executeUpdate();
		ResultSet rs = pstmt.getGeneratedKeys();
		if (rs.next()) {
			whid = rs.getInt(1);
		}
		if (whid > 0) {// on error
			JSONObject returnObject = new JSONObject();
			returnObject.put("whid", whid);
			returnObject.put("warehouse", whObject.getString("warehouse"));
			returnObject.put("type", whObject.getString("type"));
			returnObject.put("medlonode", whObject.getString("medlonode"));
			response = Response.ok().entity(returnObject.toString()).build();
		} else {// matched one found
			JSONObject errObject = new JSONObject();
			errObject.put("error", "Error creating warehouse");
			response = Response.status(401).entity(errObject.toString()).build();
		}
		rs.close();
		pstmt.close();
		con.close();

		return response;
	}

	@GET
	@Produces(MediaType.APPLICATION_JSON)
	@Path("")
	public Response retrieveAll() throws JSONException, ClassNotFoundException, SQLException {
		JSONObject whObject = new JSONObject();
		JSONArray whArray = new JSONArray();
		Response response = null;

		String selectWHSQL = "SELECT whid,warehouse,whtype,medlonode FROM warehouses ORDER BY whid";
		dbConnection con = new dbConnection();
		PreparedStatement pstmt = con.getConnection().prepareStatement(selectWHSQL);
		ResultSet rs = pstmt.executeQuery();

		while (rs.next()) {
			JSONObject whSingleObject = new JSONObject();
			whSingleObject.put("whid", rs.getInt("whid"));
			whSingleObject.put("warehouse", rs.getString("warehouse"));
			whSingleObject.put("type", rs.getString("whtype"));
			whSingleObject.put("medlonode", rs.getString("medlonode"));
			whArray.put(whSingleObject);
		}

		whObject.put("warehouses", whArray);

		response = Response.ok().entity(whObject.toString()).build();

		rs.close();
		pstmt.close();
		con.close();

		return response;
	}

	@GET
	@Produces(MediaType.APPLICATION_JSON)
	@Path("{whid}")
	public Response retrieveSingle(@PathParam("whid") int whid)
			throws JSONException, ClassNotFoundException, SQLException {
		JSONObject whObject = new JSONObject();
		Response response = null;

		String selectWHSQL = "SELECT whid,warehouse,whtype,medlonode FROM warehouses WHERE whid=?";
		dbConnection con = new dbConnection();
		PreparedStatement pstmt = con.getConnection().prepareStatement(selectWHSQL);
		pstmt.setInt(1, whid);
		ResultSet rs = pstmt.executeQuery();

		if (rs.wasNull()) {
			whObject.put("error", "Invalid warehouse");
			response = Response.status(401).entity(whObject.toString()).build();
		} else {
			while (rs.next()) {
				whObject.put("whid", rs.getInt("whid"));
				whObject.put("warehouse", rs.getString("warehouse"));
				whObject.put("type", rs.getString("whtype"));
				whObject.put("medlonode", rs.getString("medlonode"));
			}
			response = Response.ok().entity(whObject.toString()).build();
		}

		rs.close();
		pstmt.close();
		con.close();

		return response;
	}

}
