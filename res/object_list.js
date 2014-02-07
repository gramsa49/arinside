var schemaType = new Array();

function bHasTypeFilter() {
    var allTypeOn = true;
    var allTypeOff = true;
    for (i = 1; i < 6; i++) {
        schemaType[i] = $('#multiFilter input[value="' + i + '"]').attr('checked');
        if (schemaType[i]) { allTypeOff = false; }
        if (!schemaType[i]) { allTypeOn = false; }
    }
    return !(allTypeOff || allTypeOn)
}

function initTable(tableId, filterId, resultCountId) {
    var table = $('#' + tableId);
    table.data('filterid', filterId).data('resultid', resultCountId).data('maxresult', 100).data('lastmatches', 0);
}

function filterTable(tableId, appendNextChunk) {
    var table = $('#' + tableId);
    var search = $('#' + table.data('filterid')).val().replace(" +", " ").replace(" ", ".*");
    var numSearch = search.search("^\\d+$");
    var maxMatch = 0 + (table.data('maxresult'));
    var lastMatches = 0;
    var matches = 0;
    var hasTypeFilter = bHasTypeFilter();
    var hasFilter = (search != null && search.length > 0) || hasTypeFilter;
    var start = 0;
    var end = schemaList.length;
    var showAllMatches = appendNextChunk == 'all';

    if (appendNextChunk) {
        start = table.data('lastindex');
        lastMatches += table.data('lastmatches');
    }
    else {
        $('#' + tableId + ' tbody tr:gt(0)').remove();
    }

    if (hasFilter) {
        for (var i = start; i < end; i++) {
            var r = new RegExp(search, "i");
            if ((!hasTypeFilter || hasTypeFilter && schemaType[schemaList[i][5]]) && (schemaList[i][1].match(r) || (numSearch == 0 && ("" + schemaList[i][0]) == search))) {
                matches++;
                var row = ($("<tr>")
					.append($("<td>")
						.append(getIcon(rootLevel, 1, 0))
						.append($("<a>").attr("href", schemaList[i][8]).text(schemaList[i][1]))
					)
					.append($("<td>").text(schemaList[i][2]))
					.append($("<td>").text(schemaList[i][3]))
					.append($("<td>").text(schemaList[i][4]))
					.append($("<td>").text(ARSchemaType(schemaList[i][5])))
					.append($("<td>").text(schemaList[i][6]))
					.append($("<td>").text(schemaList[i][7]))
				);

                table.append(row);
            }
            if (!showAllMatches && matches >= maxMatch) {
                var row = $("<tr>")
					.append($("<td class='warn' colspan=7>").text("Result limit reached! ")
						.append($("<a id=showNext href='javascript:void(0)'>Show Next " + maxMatch + "</a>").click(function() {
						    $(this).parents('tr:first').remove();
						    filterTable(tableId, 'next');
						}))
						.append(" &nbsp; ")
						.append($("<a id=showAll href='javascript:void(0)'>Show All</a>").click(function() {
						    $(this).parents('tr:first').remove();
						    filterTable(tableId, 'all');
						}))
					);
                table.append(row);
                table.data('lastindex', i + 1);
                table.data('lastmatches', lastMatches + matches);
                break;
            }
        };
    }
    $('#' + table.data('resultid')).text((hasFilter ? "showing " + (lastMatches + matches) + " out of " : ""));
}

function initSchemaTable() {
    if (schemaList != null) { initTable('schemaList', 'formNameFilter', 'schemaListFilterResultCount'); }
}

function updateSchemaTable() {
    if (schemaList != null) { filterTable('schemaList'); }
}

$('document').ready(function() {
    var checkBoxes = $('#multiFilter input[type="checkbox"]');

    $(".clearable").on('propertychange keyup input paste', 'input.data_field', function(e) {
        if (e.keyCode == 27 /*Escape-Key*/) { $(this).val(''); }
        $(this).stopTime().oneTime(300, function() {
            $("#execFormFilter").click();
        });
    });
    $("#execFormFilter").click(function() {
        updateSchemaTable();
    });
    $("#typeFilterAll").click(function() {
        $('#multiFilter input[type="checkbox"]').each(function() {
            this.checked = true;
        });
        $("#execFormFilter").click();
    });
    $("#typeFilterNone").click(function() {
        checkBoxes.each(function() {
            this.checked = false;
        });
        $("#execFormFilter").click();
    });
    $("#typeFilterInvert").click(function() {
        checkBoxes.each(function() {
            this.checked = !this.checked;
        });
        $("#execFormFilter").click();
    });
    checkBoxes.change(function() {
        $("#execFormFilter").click();
    });

    initSchemaTable();
    if ($("#formNameFilter").val() != "" || bHasTypeFilter()) {
        $("#execFormFilter").click();
    };
});
