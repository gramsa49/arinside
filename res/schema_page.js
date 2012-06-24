$(function() {	
	$("#MainObjectTabCtrl").tabs(); 
	$("#MainObjectTabCtrl div").addClass("inner-tab");
});
$('document').ready(function() { 
	$.address.change(function(event) {
		$("#MainObjectTabCtrl").tabs( "select" , window.location.hash );
	});
	$("#MainObjectTabCtrl").bind("tabsselect", function(event, ui) {
		window.location.hash = ui.tab.hash; 
	});
});
