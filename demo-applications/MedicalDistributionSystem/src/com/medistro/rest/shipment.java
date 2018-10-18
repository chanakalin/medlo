package com.medistro.rest;

import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.text.SimpleDateFormat;

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

@Path("/shipment")
@ApplicationPath("/rest")
public class shipment extends Application {

	// create
	@PUT
	@Consumes(MediaType.APPLICATION_JSON)
	@Produces(MediaType.APPLICATION_JSON)
	@Path("")
	public Response create(String jsonShipmentData) throws JSONException, ClassNotFoundException, SQLException {
		JSONObject shipmentObject = new JSONObject(jsonShipmentData);
		System.out.println(shipmentObject.toString());
		Response response = null;
		int shipid = 0;
		int ship2whid = 0;

		String insertShipmentSQL = "INSERT INTO shipment (shipment) VALUES (?)";
		dbConnection con = new dbConnection();
		PreparedStatement pstmt = con.getConnection().prepareStatement(insertShipmentSQL);
		pstmt.setString(1, shipmentObject.getString("shipment"));
		pstmt.executeUpdate();
		ResultSet rs = pstmt.getGeneratedKeys();
		if (rs.next()) {
			shipid = rs.getInt(1);
		}
		if (shipid <= 0) {// error on shipment creation
			JSONObject errObject = new JSONObject();
			errObject.put("error", "Error creating shipment");
			response = Response.status(401).entity(errObject.toString()).build();
		} else {
			JSONObject shipObject = new JSONObject();
			shipObject.put("shipid", shipid);
			response = Response.ok().entity(shipObject.toString()).build();
		}
		rs.close();
		pstmt.close();
		//
		// shipment to WH
		if (shipid > 0) {
			String insertShipment2WHSQL = "INSERT INTO shipment2wh (shipid,whid) VALUES (?,?)";
			pstmt = con.getConnection().prepareStatement(insertShipment2WHSQL);
			pstmt.setInt(1, shipid);
			pstmt.setInt(2, shipmentObject.getInt("whid"));
			pstmt.executeUpdate();
			rs = pstmt.getGeneratedKeys();
			if (rs.next()) {
				ship2whid = rs.getInt(1);
			}
			if (shipid <= 0) {// error on shipment creation
				JSONObject errObject = new JSONObject();
				errObject.put("error", "Error creating shipment to warehouse mapping");
				response = Response.status(401).entity(errObject.toString()).build();
			}
			rs.close();
			pstmt.close();
		}
		//
		// shipment details
		if (ship2whid > 0) {
			JSONArray itemArray = shipmentObject.getJSONArray("items");
			for (int i = 0; i < itemArray.length(); i++) {
				JSONObject item = itemArray.getJSONObject(i);
				String insertShipmentDetails = "INSERT INTO shipmentdetail (shipid,itemname,tempmax,tempmin,humidmax,humidmin) VALUES (?,?,?,?,?,?)";
				pstmt = con.getConnection().prepareStatement(insertShipmentDetails);
				pstmt.setInt(1, shipid);
				pstmt.setString(2, item.getString("itemname"));
				pstmt.setInt(3, item.getInt("tempmax"));
				pstmt.setInt(4, item.getInt("tempmin"));
				pstmt.setInt(5, item.getInt("humidmax"));
				pstmt.setInt(6, item.getInt("humidmin"));
				pstmt.execute();
				pstmt.close();
			}
		}

		con.close();

		return response;
	}

	@GET
	@Produces(MediaType.APPLICATION_JSON)
	@Path("")
	public Response retrieveAll() throws JSONException, ClassNotFoundException, SQLException {
		JSONObject shipmentObject = new JSONObject();
		JSONArray shipmentArray = new JSONArray();
		Response response = null;

		String selectShipmentSQL = "SELECT shipid,shipment FROM shipment ORDER BY shipid DESC";
		dbConnection con = new dbConnection();
		PreparedStatement pstmt = con.getConnection().prepareStatement(selectShipmentSQL);
		ResultSet rs = pstmt.executeQuery();

		while (rs.next()) {
			JSONObject shipmentSingleObject = new JSONObject();
			shipmentSingleObject.put("shipid", rs.getInt("shipid"));
			shipmentSingleObject.put("shipment", rs.getString("shipment"));
			shipmentArray.put(shipmentSingleObject);
		}

		shipmentObject.put("shipment", shipmentArray);

		response = Response.ok().entity(shipmentObject.toString()).build();

		rs.close();
		pstmt.close();
		con.close();

		return response;
	}

	// shipment view
	@GET
	@Produces(MediaType.APPLICATION_JSON)
	@Path("{shipid}")
	public Response retrieveSingle(@PathParam("shipid") int shipid)
			throws JSONException, ClassNotFoundException, SQLException {
		JSONObject shipmentObject = new JSONObject();
		JSONArray shipmentWarehouseArray = new JSONArray();
		Response response = null;

		String selectShipmentSQL = "SELECT shipment FROM shipment WHERE shipid=?";
		dbConnection con = new dbConnection();
		PreparedStatement pstmt = con.getConnection().prepareStatement(selectShipmentSQL);
		pstmt.setInt(1, shipid);
		ResultSet rs = pstmt.executeQuery();
		while (rs.next()) {
			shipmentObject.put("shipid", shipid);
			shipmentObject.put("shipment", rs.getString("shipment"));
		}
		rs.close();
		pstmt.close();
		// retirve warehouses
		SimpleDateFormat simpleDateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		String selectShipmentWarehouses = "SELECT s2w.whid,wh.warehouse,wh.medlonode,s2w.fromtime FROM shipment2wh s2w INNER JOIN warehouses wh ON wh.whid=s2w.whid WHERE s2w.shipid=? ORDER BY s2w.fromtime";
		pstmt = con.getConnection().prepareStatement(selectShipmentWarehouses);
		pstmt.setInt(1, shipid);
		rs = pstmt.executeQuery();
		while (rs.next()) {
			JSONObject warehouseObject = new JSONObject();
			warehouseObject.put("whid", rs.getInt("whid"));
			warehouseObject.put("warehouse", rs.getString("warehouse"));
			warehouseObject.put("medlonode", rs.getString("medlonode"));
			warehouseObject.put("fromtime", simpleDateFormat.format(rs.getTimestamp("fromtime")));
			warehouseObject.put("fromtimeepoch", rs.getTimestamp("fromtime").getTime());
			shipmentWarehouseArray.put(warehouseObject);
		}
		shipmentObject.put("warehouse", shipmentWarehouseArray);

		response = Response.ok().entity(shipmentObject.toString()).build();

		rs.close();
		pstmt.close();
		con.close();

		return response;
	}

	// Shipment details
	@GET
	@Produces(MediaType.APPLICATION_JSON)
	@Path("items/{shipid}")
	public Response retrieveSingleData(@PathParam("shipid") int shipid)
			throws JSONException, ClassNotFoundException, SQLException {
		JSONObject itemObject = new JSONObject();
		JSONArray itemsArray = new JSONArray();
		Response response = null;

		String selectItemsSQL = "SELECT dtlid,itemname,tempmax,tempmin,humidmax,humidmin FROM shipmentdetail WHERE shipid=?";
		dbConnection con = new dbConnection();
		PreparedStatement pstmt = con.getConnection().prepareStatement(selectItemsSQL);
		pstmt.setInt(1, shipid);
		ResultSet rs = pstmt.executeQuery();
		while (rs.next()) {
			JSONObject singleItem = new JSONObject();
			singleItem.put("dtlid", rs.getInt("dtlid"));
			singleItem.put("itemname", rs.getString("itemname"));
			singleItem.put("tempmax", rs.getInt("tempmax"));
			singleItem.put("tempmin", rs.getInt("tempmin"));
			singleItem.put("humidmax", rs.getInt("humidmax"));
			singleItem.put("humidmin", rs.getInt("humidmin"));
			itemsArray.put(singleItem);
		}
		itemObject.put("item", itemsArray);
		response = Response.ok().entity(itemObject.toString()).build();

		rs.close();
		pstmt.close();
		con.close();

		return response;
	}

	// update items list
	@POST
	@Consumes(MediaType.APPLICATION_JSON)
	@Produces(MediaType.APPLICATION_JSON)
	@Path("{shipid}")
	public Response update(@PathParam("shipid") int shipid, String jsonShipmentItemData)
			throws JSONException, ClassNotFoundException, SQLException {

		JSONArray itemsUpdatedArray = new JSONObject(jsonShipmentItemData).getJSONArray("items");
		Response response = null;

		// delete existing
		String deleteShipmentItemsSQL = "DELETE FROM shipmentdetail WHERE shipid=?";
		dbConnection con = new dbConnection();
		PreparedStatement pstmt = con.getConnection().prepareStatement(deleteShipmentItemsSQL);
		pstmt.setInt(1, shipid);
		pstmt.execute();
		pstmt.close();
		// insert updated
		for (int i = 0; i < itemsUpdatedArray.length(); i++) {
			JSONObject item = itemsUpdatedArray.getJSONObject(i);
			String insertShipmentDetails = "INSERT INTO shipmentdetail (shipid,itemname,tempmax,tempmin,humidmax,humidmin) VALUES (?,?,?,?,?,?)";
			pstmt = con.getConnection().prepareStatement(insertShipmentDetails);
			pstmt.setInt(1, shipid);
			pstmt.setString(2, item.getString("itemname"));
			pstmt.setInt(3, item.getInt("tempmax"));
			pstmt.setInt(4, item.getInt("tempmin"));
			pstmt.setInt(5, item.getInt("humidmax"));
			pstmt.setInt(6, item.getInt("humidmin"));
			pstmt.execute();
			pstmt.close();
		}
		//insert warehouse transit
		String insertShipment2WHSQL = "INSERT INTO shipment2wh (shipid,whid) VALUES (?,?)";
		pstmt = con.getConnection().prepareStatement(insertShipment2WHSQL);
		pstmt.setInt(1, shipid);
		pstmt.setInt(2,  new JSONObject(jsonShipmentItemData).getInt("whid"));
		pstmt.executeUpdate();
		pstmt.close();
		con.close();
		response = Response.ok().build();
		return response;
	}

}
