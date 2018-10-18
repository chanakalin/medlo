medistroapp.controller('ctrlShipmentsEdit', function($scope, $rootScope,
		$routeParams, $cookies, $location, $http) {

	$scope.shipmentWHData = null;
	$scope.shipmentItems = null;
	$scope.shipmentId = 0;
	$scope.shipmentName = "";
	$scope.warehouses;
	$scope.warehouseName="Warehouse";
	$scope.warehouseId=0;
	$scope.medloNode="";
	
	
	//add item to items
	$scope.addNewItem = function(){
		var text = '{"itemname":"Item name","tempmax":"25","tempmin":"20","humidmax":"100","humidmin":"40"}'; 
		$scope.shipmentItems[$scope.shipmentItems.length] = JSON.parse(text);
	}
	
	//select warehouse
	$scope.selectWarehouse = function(whname,whid,medlonode) {
		$scope.warehouseName=whname;
		$scope.warehouseId=whid;
		$scope.medloNode=medlonode;
	}
	
	//remove item from item
	$scope.removeItem = function(index){
		$scope.shipmentItems.splice(index,1);
	}

	// Shipment data
	$scope.loadData = function() {
		//load warehpuses;
		$http({
			method : "GET",
			url : "rest/warehouse",
			headers : {
				"Content-Type" : "application/json; charset=utf-8"
			},
			responseType : "json"
		}).then(function successCallback(response) {
			if (response.status == 200) {// successfull retrieve
				$scope.warehouses = response.data.warehouses;
			}
		});
		// load shipment data
		$http({
			method : "GET",
			url : "rest/shipment/" + $routeParams.shipid,
			headers : {
				"Content-Type" : "application/json; charset=utf-8"
			},
			responseType : "json"
		}).then(function successCallback(response) {
			if (response.status == 200) {// successfull retrieve
				$scope.shipmentId = response.data.shipid;
				$scope.shipmentName = response.data.shipment;
				$scope.shipmentWHData = response.data.warehouse;
				$scope.warehouseName=$scope.shipmentWHData[0].warehouse;
				$scope.warehouseId=$scope.shipmentWHData[0].whid;
				$scope.medloNode=$scope.shipmentWHData[0].medlonode;
				console.log($scope.warehouseName);
				$http({
					method : "GET",
					url : "rest/shipment/items/" + $routeParams.shipid,
					headers : {
						"Content-Type" : "application/json; charset=utf-8"
					},
					responseType : "json"
				}).then(function successCallback(response) {
					if (response.status == 200) {// successfull retrieve
						$scope.shipmentItems = response.data.item;
					}
				});
			}
		});
	}
	
	//update shipment
	$scope.updateShipment = function(){
		var postingData = JSON.stringify({whid:$scope.warehouseId,items:$scope.shipmentItems,itemCount:$scope.shipmentItems.length});
		$http({
			method : "POST",
			url : "rest/shipment/" + $scope.shipmentId,
			headers : {
				"Content-Type" : "application/json; charset=utf-8"
			},
			responseType : "json",
			data	: postingData
		}).then(function successCallback(response) {
			//post env config ranges
			//get ranges
			$tempMax=$scope.shipmentItems[0].tempmax;
			$tempMin=$scope.shipmentItems[0].tempmin;
			$humidMax=$scope.shipmentItems[0].humidmax;
			$humidMin=$scope.shipmentItems[0].humidmin;
			for(var i=1;i<$scope.shipmentItems.length;i++){
				if($scope.shipmentItems[i].tempmax<$tempMax){//if current max > new max
					$tempMax = $scope.shipmentItems[i].tempmax;
				}
				if($scope.shipmentItems[i].tempmin>$tempMin){//if current min < new min
					$tempMin = $scope.shipmentItems[i].tempmin;
				}
				if($scope.shipmentItems[i].humidmax<$humidMax){//if current max > new max
					$humidMax = $scope.shipmentItems[i].humidmax;
				}
				if($scope.shipmentItems[i].humidmin>$humidMin){//if current min < new min
					$humidMin = $scope.shipmentItems[i].humidmin;
				}
			}
				//post to iot server
				$userName="admin";
				$passWord="admin";
				$http({
					method 	: "POST",
					url		: "https://medlo.trync.org:8243/api-application-registration/register",
					headers	: {
						   "Access-Control-Allow-Origin": "*",
						   "Content-Type": "application/json; charset=utf-8",
						   "authorization": "Basic "+btoa($userName+":"+$passWord)
					},
					responseType	: "json",
					data	: JSON.stringify({ applicationName:"appName", tags:["device_management"]})
				}).then(function successCallback(response) {
						$rootScope.wso2iotClientAuthBase64 = btoa(response.data.client_id+":"+response.data.client_secret);
						//console.log(response);
						$http({
							method 	: "POST",
							url		: "https://medlo.trync.org:8243/token",
							headers	: {
								   "Access-Control-Allow-Origin": "*",
								   "Content-Type": "application/x-www-form-urlencoded; charset=utf-8",
								   "authorization": "Basic "+$rootScope.wso2iotClientAuthBase64
							},
							responseType	: "json",
							data	: "grant_type=password&username="+$userName+"&password="+$passWord+"&scope=perm:medlonode:enroll perm:sign-csr perm:admin:devices:view perm:roles:add perm:roles:add-users perm:roles:update perm:roles:permissions perm:roles:details perm:roles:view perm:roles:create-combined-role perm:roles:delete perm:get-activity perm:devices:delete perm:devices:applications perm:devices:effective-policy perm:devices:compliance-data perm:devices:features perm:devices:operations perm:devices:search perm:devices:details perm:devices:update perm:devices:view perm:view-configuration perm:manage-configuration perm:policies:remove perm:policies:priorities perm:policies:deactivate perm:policies:get-policy-details perm:policies:manage perm:policies:activate perm:policies:update perm:policies:changes perm:policies:get-details perm:users:add perm:users:details perm:users:count perm:users:delete perm:users:roles perm:users:user-details perm:users:credentials perm:users:search perm:users:is-exist perm:users:update perm:users:send-invitation perm:admin-users:view perm:groups:devices perm:groups:update perm:groups:add perm:groups:device perm:groups:devices-count perm:groups:remove perm:groups:groups perm:groups:groups-view perm:groups:share perm:groups:count perm:groups:roles perm:groups:devices-remove perm:groups:devices-add perm:groups:assign perm:device-types:features perm:device-types:types perm:applications:install perm:applications:uninstall perm:admin-groups:count perm:admin-groups:view perm:notifications:mark-checked perm:notifications:view perm:admin:certificates:delete perm:admin:certificates:details perm:admin:certificates:view perm:admin:certificates:add perm:admin:certificates:verify"
						}).then(function successCallback(response) {
							//console.log(response);
							$rootScope.wso2iotAccessToken = response.data.access_token;
							$rootScope.wso2iotRefreshToken = response.data.refresh_token;
							console.log($rootScope.wso2iotAccessToken);
							//retrieve device to the group medistro
							//$scope.getSensorData("qdvkplqxil2w","temperature",1539213086,1539214086);
							//
							//
							
								$http({
									method 	: "POST",
									url		: "https://medlo.trync.org:8243/medlonode/1.0.0/device/"+$scope.medloNode+"/configranges?"+"max-temperature="+$tempMax+"&min-temperature="+$tempMin+"&max-humidity="+$humidMax+"&min-humidity="+$humidMin,
									headers	: {
											"Content-Type":"x-www-form-urlencoded",
										   "Access-Control-Allow-Origin": "*",
										   "Access-Control-Allow-Headers": "authorization,Access-Control-Allow-Origin",
										   "authorization": "Bearer "+$rootScope.wso2iotAccessToken
									},
									responseType	: "json",
									data			: "max-temperature="+$tempMax+"&min-temperature="+$tempMin+"&max-humidity="+$humidMax+"&min-humidity="+$humidMin
								}).then(function successCallback(response) {
										console.log("Posted new paramataes to IoT server "+response.status);
										$location.path("/shipments");
								});
								//posting to iot server end
						});
				});
				//post to iot server end
		});
	}

});