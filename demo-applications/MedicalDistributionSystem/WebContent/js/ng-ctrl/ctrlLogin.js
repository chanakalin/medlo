medistroapp.controller('ctrlLogin', function($scope,$rootScope,$cookies,$location,$http){
	
	//login
	$scope.login = function(){
		$http({
				method 	: "POST",
				url		: "rest/user/login",
				headers	: {
					   "Content-Type": "application/json; charset=utf-8"
				},
				responseType	: "json",
				data	: JSON.stringify({ username:$scope.username, password:$scope.password})
		}).then(function successCallback(response) {
				if(response.status==200){//successfull login
					$cookies.put('userid',response.data.userid);
					$cookies.put('username',response.data.username);
					$cookies.put('usrlevel',response.data.userlevel);
					$rootScope.loggedUserId = $cookies.get("userid");
					$rootScope.loggedUserName = $cookies.get("username");
					$rootScope.loggedUserLevel = $cookies.get("userlevel");
					$location.path("/home");
				}
		  });
	}
	
});