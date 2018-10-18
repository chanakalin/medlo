medistroapp.controller('ctrlWarehousesAnalyze', function($scope, $rootScope,$routeParams, $cookies,
		$location, $http) {
	
	$scope.warehouse=null;
	$scope.temperatureData=null;
	$scope.humidityData=null;
	$scope.uvindexData=null;
	
	//WH data
	$scope.loadData = function(){
		//load wh data
		$http({
			method : "GET",
			url : "rest/warehouse/"+$routeParams.whid,
			headers : {
				"Content-Type" : "application/json; charset=utf-8"
			},
			responseType : "json"
		}).then(function successCallback(response) {
			if (response.status == 200) {// successfull retrieve
				$scope.warehouse = response.data;
				//set defaults
				$today=new Date();
				$scope.fromdate = new Date($today.getFullYear(),$today.getMonth(),$today.getDate(),0,0,0,0);
				$scope.todate = new Date($today.getFullYear(),$today.getMonth(),$today.getDate()+1,0,0,0,0);
				//load charts
				$scope.reloadCharts();
			}
		});
	}
	
		
	
	//get available devices
	$scope.getSensorData = function($deviceId,$sensor,$from,$to){
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
				$scope.returnData = new Array($scope.returnData);
				for(var i=0;i<response.data.length;i++){
					$singleRecord = new Array(2);
					$dateTime = new Date(0);
					$dateTime.setUTCSeconds(response.data[i].values.meta_time);
					$singleRecord[0] = $dateTime;
					if($sensor=="temperature"){
						$singleRecord[1] = response.data[i].values.temperature;
					}
					$scope.returnData[i]=$singleRecord;
				}				
				if($sensor=="temperature"){
					$scope.temperatureData = $scope.returnData;
				}
		});
	}
	
	
	//reload charts
	$scope.reloadCharts = function(){
		$fromdate = Math.round($scope.fromdate.getTime()/1000);
		$todate = Math.round($scope.todate.getTime()/1000);
		console.log("Generating charts from "+$fromdate+" to "+$todate);
		$scope.generateCharts($fromdate,$todate);
	}
	
	//generate charts
	$scope.generateCharts = function($fromEpoch,$toEpoch){
		console.log("Loading medlo node data of "+$scope.warehouse.medlonode);				
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
					//temperature data
					$http({
						method 	: "GET",
						url		: "https://medlo.trync.org:8243/medlonode/1.0.0/device/stats/"+$scope.warehouse.medlonode+"?sensorType=temperature&from="+$fromEpoch+"&to="+$toEpoch,
						headers	: {
							   "Access-Control-Allow-Origin": "*",
							   "Access-Control-Allow-Headers": "authorization,Access-Control-Allow-Origin",
							   "authorization": "Bearer "+$rootScope.wso2iotAccessToken
						},
						responseType	: "json"
					}).then(function successCallback(response) {
							$scope.temperatureData = new Array(response.data.length);
							for(var i=0;i<response.data.length;i++){
								$singleRecord = new Array(2);
								//$dateTime = new Date(0);
								//$dateTime.setUTCSeconds(response.data[i].values.meta_time);
								//gilmart
								$dateTime = new Date((response.data[i].values.meta_time*1000)-(Math.round((60*60*5.5))*1000));
								//console.log($dateTime);
								$singleRecord[0] = Date.UTC($dateTime.getFullYear(),$dateTime.getMonth(),$dateTime.getDate(),$dateTime.getHours(),$dateTime.getMinutes(),$dateTime.getSeconds());
								$singleRecord[1] = response.data[i].values.temperature;
								$scope.temperatureData[i]=$singleRecord;
							}	
							console.log($scope.temperatureData);
							Highcharts.chart('chartTemperature', {
							    chart: {type: 'spline'},
							    title: {text: 'Temperature'},
							    subtitle: {text: 'Temperature data from medlo node '+$scope.warehouse.medlonode},
							    xAxis: {
							        type: 'datetime',
							        dateTimeLabelFormats: { // don't display the dummy year
							            month: '%e. %b',
							            year: '%b'
							        },
							        title: {
							            text: 'Date'
							        }
							    },
							    yAxis: {
							        title: {
							            text: 'Temperature (°C)'
							        },
							        min: 0
							    },
							    tooltip: {
							        headerFormat: '<b>{series.name}</b><br>',
							        pointFormat: '{point.x:%Y-%b-%e %H:%M:%S}: {point.y:.2f} °C'
							    },

							    plotOptions: {
							        spline: {
							            marker: {
							                enabled: true
							            }
							        }
							    },

							    colors: ['#5b68e5'],

							    // Define the data points. All series have a dummy year
							    // of 1970/71 in order to be compared on the same x axis. Note
							    // that in JavaScript, months start at 0 for January, 1 for February etc.
							    series: [{
							        name: "Temperature",
							        data: $scope.temperatureData}]
							});
							
					});
					//temperature end
					//
					//
					//himidity data
					$http({
						method 	: "GET",
						url		: "https://medlo.trync.org:8243/medlonode/1.0.0/device/stats/"+$scope.warehouse.medlonode+"?sensorType=humidity&from="+$fromEpoch+"&to="+$toEpoch,
						headers	: {
							   "Access-Control-Allow-Origin": "*",
							   "Access-Control-Allow-Headers": "authorization,Access-Control-Allow-Origin",
							   "authorization": "Bearer "+$rootScope.wso2iotAccessToken
						},
						responseType	: "json"
					}).then(function successCallback(response) {
							$scope.humidityData = new Array(response.data.length);
							for(var i=0;i<response.data.length;i++){
								$singleRecord = new Array(2);
								//$dateTime = new Date(0);
								//$dateTime.setUTCSeconds(response.data[i].values.meta_time);
								$dateTime = new Date((response.data[i].values.meta_time*1000)-(Math.round((60*60*5.5))*1000));
								console.log($dateTime);
								$singleRecord[0] = Date.UTC($dateTime.getFullYear(),$dateTime.getMonth(),$dateTime.getDate(),$dateTime.getHours(),$dateTime.getMinutes(),$dateTime.getSeconds());
								$singleRecord[1] = response.data[i].values.humidity;
								$scope.humidityData[i]=$singleRecord;
							}	
							Highcharts.chart('chartHumidity', {
							    chart: {type: 'spline'},
							    title: {text: 'Humidity'},
							    subtitle: {text: 'Humidity data from medlo node '+$scope.warehouse.medlonode},
							    xAxis: {
							        type: 'datetime',
							        dateTimeLabelFormats: { // don't display the dummy year
							            month: '%e. %b',
							            year: '%b'
							        },
							        title: {
							            text: 'Date'
							        }
							    },
							    yAxis: {
							        title: {
							            text: 'Humidity (%)'
							        },
							        min: 0
							    },
							    tooltip: {
							        headerFormat: '<b>{series.name}</b><br>',
							        pointFormat: '{point.x:%Y-%b-%e %H:%M:%S}: {point.y:.2f} %'
							    },

							    plotOptions: {
							        spline: {
							            marker: {
							                enabled: true
							            }
							        }
							    },

							    colors: ['#40b844'],
							    series: [{
							        name: "Humidity",
							        data: $scope.humidityData}]
							});
							
					});
					//humidity end
					//
					//
					//uvindex data
					$http({
						method 	: "GET",
						url		: "https://medlo.trync.org:8243/medlonode/1.0.0/device/stats/"+$scope.warehouse.medlonode+"?sensorType=uvindex&from="+$fromEpoch+"&to="+$toEpoch,
						headers	: {
							   "Access-Control-Allow-Origin": "*",
							   "Access-Control-Allow-Headers": "authorization,Access-Control-Allow-Origin",
							   "authorization": "Bearer "+$rootScope.wso2iotAccessToken
						},
						responseType	: "json"
					}).then(function successCallback(response) {
						$scope.uvindexData = new Array(response.data.length);
							for(var i=0;i<response.data.length;i++){
								$singleRecord = new Array(2);
								//$dateTime = new Date(0);
								//$dateTime.setUTCSeconds(response.data[i].values.meta_time);
								$dateTime = new Date((response.data[i].values.meta_time*1000)-(Math.round((60*60*5.5))*1000));
								console.log($dateTime);
								$singleRecord[0] = Date.UTC($dateTime.getFullYear(),$dateTime.getMonth(),$dateTime.getDate(),$dateTime.getHours(),$dateTime.getMinutes(),$dateTime.getSeconds());
								$singleRecord[1] = response.data[i].values.uvindex/100;
								$scope.uvindexData[i]=$singleRecord;
							}	
							Highcharts.chart('chartUVIndex', {
							    chart: {type: 'spline'},
							    title: {text: 'UVIndex'},
							    subtitle: {text: 'UV Index data from medlo node '+$scope.warehouse.medlonode},
							    xAxis: {
							        type: 'datetime',
							        dateTimeLabelFormats: { // don't display the dummy year
							            month: '%e. %b',
							            year: '%b'
							        },
							        title: {
							            text: 'Date'
							        }
							    },
							    yAxis: {
							        title: {
							            text: 'UV Index'
							        },
							        min: 0
							    },
							    tooltip: {
							        headerFormat: '<b>{series.name}</b><br>',
							        pointFormat: '{point.x:%Y-%b-%e %H:%M:%S}:   {point.y:.2f}'
							    },

							    plotOptions: {
							        spline: {
							            marker: {
							                enabled: true
							            }
							        }
							    },

							    colors: ['#ce83fb'],
							    series: [{
							        name: "UVIndex",
							        data: $scope.uvindexData}]
							});
							
					});
					//uvindex end
					//
				});
		});
	}


});