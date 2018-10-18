medistroapp.controller('ctrlWarehouses', function($scope, $rootScope, $cookies,
		$location, $http) {
	
	$scope.warehouses;
	$scope.whtype="Fixed";
	$scope.medloNodeName="";
	$scope.medloNodeId="";
	
	$scope.redirectToCreate = function() {
		$location.path("/warehouses/create");
	}

	//load wh list
	$scope.loadwh = function() {
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
				console.log($scope.warehouses);
			}
		});
	}
	
});