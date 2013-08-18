$(function() {
    $("#MainObjectTabCtrl").tabs();
    $('#MainObjectTabCtrl div[id^="tab"]').addClass("inner-tab");
});

function showHideClearButton(obj) {
	var visible = $(obj).val().length ? 1 : 0 ;
	return $(obj).next('.icon_clear').stop().fadeTo(300,visible);
}

$('document').ready(function() {
    $.address.change(function(event) {
        $("#MainObjectTabCtrl").tabs("select", window.location.hash);
    });
    $("#MainObjectTabCtrl").bind("tabsselect", function(event, ui) {
        window.location.hash = ui.tab.hash;
    });
    $(".clearable").on('propertychange keyup input paste', 'input.data_field', function(){
		showHideClearButton(this).stopTime().oneTime(300, function() {
            $("#execFieldFilter").click();
        });
	}).on('click', '.icon_clear', function() {
		$(this).delay(300).fadeTo(300,0).prev('input').val('').focus().stopTime().oneTime(300, function() {
            $("#execFieldFilter").click();
        });
	});

    $("#execFieldFilter").click(function() {
        if (schemaFieldList != null) {
            var table_name = 'fieldListAll';
            var table = $('#' + table_name);
            var search = $("#fieldNameFilter").val().replace(" +", " ").replace(" ", ".*");
            var numSearch = search.search("^\\d+$");
            //var table = $("<table class='TblObjectList'><th>Field Name</th><th>Field ID</th><th>Datatype</th><th>Real Field</th><th>In Views</th><th>Modified</th><th>By</th></tr></table>");
            var matches = 0;
            var hasRealFieldColumn = ($('#' + table_name + ' tbody tr:eq(0) th:eq(3)').text().lastIndexOf("Real Field", 0) === 0);

            $('#' + table_name + ' tbody tr:gt(0)').remove();

            $.each(schemaFieldList, function(i) {
                var r = new RegExp(search, "i");
                if (schemaFieldList[i][1].match(r) || (numSearch == 0 && ("" + schemaFieldList[i][0]).match(search))) {
                    matches++;
                    table.append("<tr><td><a href='" + schemaFieldList[i][6] + "'>" + schemaFieldList[i][1] + "</a></td><td>" + schemaFieldList[i][0] + "</td><td>" + ARFieldDataTypeToString(schemaFieldList[i][2]) + (hasRealFieldColumn ? "<td></td>" : "") + "<td>" + schemaFieldList[i][3] + "</td><td>" + schemaFieldList[i][4] + "</td><td>" + schemaFieldList[i][5] + "</td></tr>");
                }
            });
            $('#fieldListFilterResultCount').text((search != null && search.length > 0 ? "showing " + matches + " out of " : ""));
        }
    });
	if ($("#fieldNameFilter").val() != "") {
		showHideClearButton($("#fieldNameFilter"));
		$("#execFieldFilter").click();
	}
});
