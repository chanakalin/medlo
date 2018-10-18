medistroapp.controller('ctrlShipments', function($scope, $rootScope, $cookies,
		$location, $http) {
	
	$scope.shipments=null;

	
	$scope.redirectToCreate = function() {
		$location.path("/shipments/create");
	}

	//load shipment list
	$scope.loadShipment = function() {
		$http({
			method : "GET",
			url : "rest/shipment",
			headers : {
				"Content-Type" : "application/json; charset=utf-8"
			},
			responseType : "json"
		}).then(function successCallback(response) {
			if (response.status == 200) {// successfull retrieve
				$scope.shipments = response.data.shipment;
				console.log($scope.shipments);
			}
		});
	}
	
});