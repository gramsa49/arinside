$(function() {
    $("#MainObjectTabCtrl").tabs();
    $('#MainObjectTabCtrl div[id^="tab"]').addClass("inner-tab");
});

$.fn.appendText = function(text) {
    this.each(function() {
        var textNode = document.createTextNode(text);
        $(this).append(textNode);
    });
};

var schemaFieldManager = {
    view_vendor_RFR: function(row) {
        return (schemaFieldList[row].length > 7 ? schemaFieldList[row][7] : "");
    },
    join_RFR: function(row) {
        var pos = 7;
        var div = $("<div>");
        var first = 0;
        for (pos; pos + 3 < schemaFieldList[row].length; pos += 4) {
            if (first > 0) { div.append($("<br/>")); }

            div.append($("<a>").attr("href", schemaFieldList[row][pos + 1]).text(schemaFieldList[row][pos]));
            div.appendText("\u00a0 -> \u00a0");
            div.append($("<img>").attr("width", 16).attr("height", 16).attr("alt", "schema.gif").attr("src", "../../img/schema.gif"));
            div.append($("<a>").attr("href", schemaFieldList[row][pos + 3]).text(schemaFieldList[row][pos + 2]));

            first++;
        }
        if (first > 0) return div;
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
    $(".clearable").on('propertychange keyup input paste', 'input.data_field', function(e) {
        if (e.keyCode == 27 /*Escape-Key*/) { $(this).val(''); }
        $(this).stopTime().oneTime(300, function() {
            $("#execFieldFilter").click();
        });
    });
    $("#schemaProperties").accordion({ heightStyle: "content", collapsible: true, active: false, animate: false });
    $("#schemaPermissions").accordion({ heightStyle: "content", collapsible: true, active: false, animate: false });

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
                    var row = ($("<tr>")
						.append($("<td>")
							.append($("<a>").attr("href", schemaFieldList[i][6]).text(schemaFieldList[i][1]))
						)
						.append($("<td>").text(schemaFieldList[i][0]))
						.append($("<td>").text(ARFieldDataTypeToString(schemaFieldList[i][2])))
					);
                    if (hasRealFieldColumn)
                        row.append($("<td>").append(schemaFieldManager.renderRealField(i)));

                    row.append($("<td>").text(schemaFieldList[i][3]))
					   .append($("<td>").text(schemaFieldList[i][4]))
					   .append($("<td>").text(schemaFieldList[i][5]));

                    table.append(row);
                }
            });
            $('#fieldListFilterResultCount').text((search != null && search.length > 0 ? "showing " + matches + " out of " : ""));
        }
    });
    if ($("#fieldNameFilter").val() != "") {
        $("#execFieldFilter").click();
    }
});
