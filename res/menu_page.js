$(function() {
	$("#MainObjectTabCtrl").tabs();
	$('#MainObjectTabCtrl div[id^="tab"]').addClass("inner-tab");
	$("#menuProperties").accordion({ heightStyle: "content", collapsible: true, active: false, animate: false });
});
