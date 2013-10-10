$(function() {
    $("#MainObjectTabCtrl").tabs();
    $('#MainObjectTabCtrl div[id^="tab"]').addClass("inner-tab");
});

var schemaFieldManager = {
    view_vendor_RFR: function(row) {
        return (schemaFieldList[row].length > 7 ? schemaFieldList[row][7] : "");
    },
    join_RFR: function(row) {
        var pos = 7;
        var result = "";
        for (pos; pos + 3 < schemaFieldList[row].length; pos += 4) {
            if (result.length > 0) { result += "<br/>"; }
            result += "<a href='" + schemaFieldList[row][pos + 1] + "'>" + schemaFieldList[row][pos] + "</a>&nbsp;-&gt;&nbsp;<img width=16 height=16 alt='schema.gif' src='../../img/schema.gif'/><a href='" + schemaFieldList[row][pos + 3] + "'>" + schemaFieldList[row][pos + 2] + "</a>";
        }
        if (result) return result;
        return (schemaFieldList[row].length > 7 ? schemaFieldList[row][7] : "");
    },
    renderRealField: function(row) {
        if (this.realFieldRenderer == null) { return ""; }
        return this.realFieldRenderer(row);
    },
    realFieldRenderer: null,
    setRenderer: function(type) {
        if (type === "view" || type === "vendor")
            this.realFieldRenderer = this.view_vendor_RFR;
        if (type === "join")
            this.realFieldRenderer = this.join_RFR;
    }
};

$('document').ready(function() {
    $.address.change(function(event) {
        $("#MainObjectTabCtrl").tabs("select", window.location.hash);
    });
    $("#MainObjectTabCtrl").bind("tabsselect", function(event, ui) {
        window.location.hash = ui.tab.hash;
    });
    $("#fieldNameFilter").keyup(function() {
        $(this).stopTime();
        $(this).oneTime(300, function() {
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
                    table.append("<tr><td><a href='" + schemaFieldList[i][6] + "'>" + schemaFieldList[i][1] + "</a></td><td>" + schemaFieldList[i][0] + "</td><td>" + ARFieldDataTypeToString(schemaFieldList[i][2]) + (hasRealFieldColumn ? "<td>" + schemaFieldManager.renderRealField(i) + "</td>" : "") + "<td>" + schemaFieldList[i][3] + "</td><td>" + schemaFieldList[i][4] + "</td><td>" + schemaFieldList[i][5] + "</td></tr>");
                }
            });
            $('#fieldListFilterResultCount').text((search != null && search.length > 0 ? "showing " + matches + " out of " : ""));
        }
    });
	if ($("#fieldNameFilter").val() != "") {
		$("#execFieldFilter").click();
	}
});
