medistroapp.controller('ctrlHome',
		function($scope, $rootScope, $cookies, $location, $http) {

			$scope.initHome = function() {
				if ($cookies.get("userid") == undefined
						|| $cookies.get("userid") <= 0) {
					console.log("Redirecting to login");
					$location.path("/login");
				}else{
					$scope.getSensorData("qdvkplqxil2w","temperature",1539213086,(Math.round(new Date().getTime()/1000)+(60*60*6)));
				}
			}

			
			
			//get available devices
			$scope.getSensorData = function($deviceId,$sensor,$from,$to){
				console.log($rootScope.wso2iotAccessToken);
				$http({
					method 	: "GET",
					url		: "https://medlo.trync.org:8243/medlonode/1.0.0/device/stats/"+$deviceId+"?sensorType="+$sensor+"&from="+$from+"&to="+$to,
					headers	: {
						   "Access-Control-Allow-Origin": "*",
						   "Access-Control-Allow-Headers": "authorization,Access-Control-Allow-Origin",
						   "authorization": "Bearer "+$rootScope.wso2iotAccessToken
					},
					responseType	: "json"
				}).then(function successCallback(response) {
						console.log(response.data);
				});
			}
			

		});