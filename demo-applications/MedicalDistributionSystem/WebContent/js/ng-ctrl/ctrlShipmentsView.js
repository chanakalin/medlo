medistroapp.controller('ctrlShipmentsView', function($scope, $rootScope,$routeParams, $cookies,
		$location, $http) {
	
	$scope.shipmentWHData=null;
	$scope.shipmentId = 0;
	$scope.shipmentName = "";
	
	//chart data
	$scope.dataTemperature=[];
	$scope.dataHumidity=[];
	$scope.dataUVIndex=[];
	
	//Shipment data
	$scope.loadData = function(){
		//load shipment data
		$http({
			method : "GET",
			url : "rest/shipment/"+$routeParams.shipid,
			headers : {
				"Content-Type" : "application/json; charset=utf-8"
			},
			responseType : "json"
		}).then(function successCallback(response) {
			if (response.status == 200) {//successfull retrieve
				$scope.shipmentId = response.data.shipid;
				$scope.shipmentName = response.data.shipment;
				$scope.shipmentWHData = response.data.warehouse;
				//generate charts
				$scope.grabSensorData();
			}
		});
	}
	
	//grab sensor data
	$scope.grabSensorData = function(){
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
					for(var i=0;i<$scope.shipmentWHData.length;i++){
						console.log("Loading medlo node data of "+$scope.shipmentWHData[i].medlonode);				
						$fromdate = $scope.shipmentWHData[i].fromtimeepoch;
						$todate = null;
						if((i+1)==$scope.shipmentWHData.length){
							$todate=new Date().getTime();
						}else{
							$todate = $scope.shipmentWHData[i+1].fromtimeepoch;
						}
						//gilmart
						$fromdate = Math.round(((60*60*5.5*1000)+$fromdate)/1000);
						$todate = Math.round(((60*60*5.5*1000)+$todate)/1000);
						//temperature data
						$http({
							method 	: "GET",
							url		: "https://medlo.trync.org:8243/medlonode/1.0.0/device/stats/"+$scope.shipmentWHData[i].medlonode+"?sensorType=temperature&from="+$fromdate+"&to="+$todate,
							headers	: {
								   "Access-Control-Allow-Origin": "*",
								   "Access-Control-Allow-Headers": "authorization,Access-Control-Allow-Origin",
								   "authorization": "Bearer "+$rootScope.wso2iotAccessToken
							},
							responseType	: "json"
						}).then(function successCallback(response) {
								if(response.data != null){
									$scope.temperatureData = new Array(response.data.length);
									for(var i=0;i<response.data.length;i++){
										$singleRecord = new Array(2);
										//$dateTime = new Date(0);
										//$dateTime.setUTCSeconds(response.data[i].values.meta_time);
										$dateTime = new Date((response.data[i].values.meta_time*1000)-(Math.round((60*60*5.5))*1000));
										//console.log($dateTime);
										$singleRecord[0] = Date.UTC($dateTime.getFullYear(),$dateTime.getMonth(),$dateTime.getDate(),$dateTime.getHours(),$dateTime.getMinutes(),$dateTime.getSeconds());
										$singleRecord[1] = response.data[i].values.temperature;
										$scope.temperatureData[i]=$singleRecord;
									}	
									$scope.dataTemperature.push.apply($scope.dataTemperature,$scope.temperatureData);	
								}
						});
						//temperature end
						//humidity data
						$http({
							method 	: "GET",
							url		: "https://medlo.trync.org:8243/medlonode/1.0.0/device/stats/"+$scope.shipmentWHData[i].medlonode+"?sensorType=humidity&from="+$fromdate+"&to="+$todate,
							headers	: {
								   "Access-Control-Allow-Origin": "*",
								   "Access-Control-Allow-Headers": "authorization,Access-Control-Allow-Origin",
								   "authorization": "Bearer "+$rootScope.wso2iotAccessToken
							},
							responseType	: "json"
						}).then(function successCallback(response) {
								if(response.data != null){
									$scope.humidityData = new Array(response.data.length);
									for(var i=0;i<response.data.length;i++){
										$singleRecord = new Array(2);
										//$dateTime = new Date(0);
										//$dateTime.setUTCSeconds(response.data[i].values.meta_time);
										$dateTime = new Date((response.data[i].values.meta_time*1000)-(Math.round((60*60*5.5))*1000));
										//console.log($dateTime);
										$singleRecord[0] = Date.UTC($dateTime.getFullYear(),$dateTime.getMonth(),$dateTime.getDate(),$dateTime.getHours(),$dateTime.getMinutes(),$dateTime.getSeconds());
										$singleRecord[1] = response.data[i].values.humidity;
										$scope.humidityData[i]=$singleRecord;
									}	
									$scope.dataHumidity.push.apply($scope.dataHumidity,$scope.humidityData);	
								}
						});
						//humidity end
						//
						//uvindex data
						$http({
							method 	: "GET",
							url		: "https://medlo.trync.org:8243/medlonode/1.0.0/device/stats/"+$scope.shipmentWHData[i].medlonode+"?sensorType=uvindex&from="+$fromdate+"&to="+$todate,
							headers	: {
								   "Access-Control-Allow-Origin": "*",
								   "Access-Control-Allow-Headers": "authorization,Access-Control-Allow-Origin",
								   "authorization": "Bearer "+$rootScope.wso2iotAccessToken
							},
							responseType	: "json"
						}).then(function successCallback(response) {
								if(response.data != null){
									$scope.uvindexData = new Array(response.data.length);
									for(var i=0;i<response.data.length;i++){
										$singleRecord = new Array(2);
										//$dateTime = new Date(0);
										//$dateTime.setUTCSeconds(response.data[i].values.meta_time);
										$dateTime = new Date((response.data[i].values.meta_time*1000)-(Math.round((60*60*5.5))*1000));
										//console.log($dateTime);
										$singleRecord[0] = Date.UTC($dateTime.getFullYear(),$dateTime.getMonth(),$dateTime.getDate(),$dateTime.getHours(),$dateTime.getMinutes(),$dateTime.getSeconds());
										$singleRecord[1] = response.data[i].values.uvindex/100;
										$scope.uvindexData[i]=$singleRecord;
									}	
									$scope.dataUVIndex.push.apply($scope.dataUVIndex,$scope.uvindexData);	
								}
								//generate charts
								$scope.generateCharts();
						});
						//uvindex data end
					}
					console.log($scope.dataTemperature);
					console.log($scope.dataHumidity);
					console.log($scope.dataUVIndex);		
				});
		});
					
	}
	
	//generate charts
	$scope.generateCharts = function(){
		//temperature
		Highcharts.chart('chartTemperature', {
		    chart: {type: 'spline'},
		    title: {text: 'Temperature'},
		    subtitle: {text: 'Temperature data for shipment '+$scope.shipmentName},
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
		    series: [{
		        name: "Temperature",
		        data: $scope.dataTemperature}]
		});
		//temperature end
		//humidity
		Highcharts.chart('chartHumidity', {
		    chart: {type: 'spline'},
		    title: {text: 'Humidity'},
		    subtitle: {text: 'Humidity data for shipment '+$scope.shipmentName},
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
		        data: $scope.dataHumidity}]
		});
		//humidity end
		//uvindex
		Highcharts.chart('chartUVIndex', {
		    chart: {type: 'spline'},
		    title: {text: 'UVIndex'},
		    subtitle: {text: 'UV Index data for shipment '+$scope.shipmentName},
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
		        data: $scope.dataUVIndex}]
		});
		//uvindex end
	}
	


});