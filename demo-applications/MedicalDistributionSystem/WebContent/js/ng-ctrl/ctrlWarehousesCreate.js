medistroapp.controller('ctrlWarehousesCreate', function($scope, $rootScope, $cookies,
		$location, $http) {
	
	$scope.medloNodes;
	$scope.whtype="Fixed";
	$scope.medloNodeName="";
	$scope.medloNodeId="";
	
	//init load medlo nodes
	$scope.loadMeloNodes = function(){
		console.log("Loading medlo nodes");
		
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
					$http({
						method 	: "GET",
						url		: "https://medlo.trync.org:8243/api/device-mgt/v1.0/groups/id/3/devices",
						headers	: {
							   "Access-Control-Allow-Origin": "*",
							   "Access-Control-Allow-Headers": "authorization,Access-Control-Allow-Origin",
							   "authorization": "Bearer "+$rootScope.wso2iotAccessToken
						},
						responseType	: "json"
					}).then(function successCallback(response) {
						$scope.medloNodes = response.data.devices;
						console.log($scope.medloNodes);
					});
				});
		});
	}
	
	
	//select wh type
	$scope.selectWHType = function(item) {
		$scope.whtype=item;
	}
	
	
	//select medlo node
	$scope.selectMedloNode = function(nodename,nodeid) {
		$scope.medloNodeName=nodename;
		$scope.medloNodeId=nodeid;
	}
	
	
	//create ware house
	$scope.createWH = function() {
		$http({
			method 	: "PUT",
			url		: "rest/warehouse",
			headers	: {
				   "Content-Type": "application/json; charset=utf-8",
			},
			responseType	: "json",
			data	: JSON.stringify({warehouse:$scope.warehouse,type:$scope.whtype.charAt(0),medlonode:$scope.medloNodeId})
		}).then(function successCallback(response) {
			console.log(response.data);
			$location.path("/warehouses");
		});
	}
	
	


});