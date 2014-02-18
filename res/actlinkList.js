function bHasTypeFilter() {
	return false;
}

function initTable(tableId, filterId, resultCountId) {
    var table = $('#' + tableId);
    table.data('filterid', filterId).data('resultid', resultCountId).data('maxresult', 100).data('lastmatches', 0);
}

function createMoreResultsRow(tableId, maxMatch) {
    return $("<tfoot>").append($("<tr>")
    .append($("<td class='warn' colspan=9>").text("Result limit reached! ")
	    .append($("<a id=showNext href='javascript:void(0)'>Show Next " + maxMatch + "</a>").click(function() {
	        $(this).parents('tfoot:first').remove();
	        filterTable(tableId, 'next');
	    }))
	    .append(" &nbsp; ")
	    .append($("<a id=showAll href='javascript:void(0)'>Show All</a>").click(function() {
	        $(this).parents('tfoot:first').remove();
	        filterTable(tableId, 'all');
	    }))
	));
}

function filterTable(tableId, appendNextChunk) {
    var table = $('#' + tableId);
    var search = $('#' + table.data('filterid')).val().replace(/ +/g, " ").replace(/ /g, ".*");
    var numSearch = search.search("^\\d+$");
    var maxMatch = 0 + (table.data('maxresult'));
    var lastMatches = 0;
    var matches = 0;
    var hasTypeFilter = bHasTypeFilter();
    var hasFilter = (search != null && search.length > 0) || hasTypeFilter;
    var start = 0;
    var end = alList.length;
    var showAllMatches = appendNextChunk == 'all';

    if (appendNextChunk) {
        start = table.data('lastindex');
        lastMatches += table.data('lastmatches');
    }
    else {
        $('#' + tableId + ' tbody,tfoot').remove();
    }

    if (hasFilter) {
        for (var i = start; i < end; i++) {
            var r = new RegExp(search, "i");
            if ((!hasTypeFilter || hasTypeFilter && schemaType[alList[i][5]]) && (alList[i][0].match(r) || (numSearch == 0 && ("" + alList[i][0]) == search))) {
                matches++;
                table.append(createActlinkRowHtml(alList[i]));
            }
            if (!showAllMatches && matches >= maxMatch) {
                table.append(createMoreResultsRow(tableId, maxMatch));
                table.data('lastindex', i + 1);
                table.data('lastmatches', lastMatches + matches);
                break;
            }
        };
    }
    $('#' + table.data('resultid')).text((hasFilter ? "showing " + (lastMatches + matches) + " out of " : ""));
}

function initActlinkTable() {
    if (alList != null) { initTable('alList', 'actlinkFilter', 'actlinkListFilterResultCount'); }
}

function updateActlinkTable() {
    if (alList != null) { filterTable('alList'); }
}

function createActlinkRowHtml(data) {
    return ($("<tr>")
        .append($("<td>")
            .append(getIcon(rootLevel, 6))
            .append($("<a>").attr("href", data[9]).text(data[0]))
        )
        .append($("<td>").text(AREnabled(data[1])))
        .append($("<td>").text(data[2]))
		.append($("<td>").text(data[3]))
		.append($("<td>").text(data[4]))
		.append($("<td>").text(data[5]))
		.append($("<td>").text(data[6]))
		.append($("<td>").text(data[7]))
		.append($("<td>").text(data[8]))
    );
}

$('document').ready(function() {
    //var checkBoxes = $('#multiFilter input[type="checkbox"]');

    $(".clearable").on('propertychange keyup input paste', 'input.data_field', function(e) {
        if (e.keyCode == 27 /*Escape-Key*/) { $(this).val(''); }
        $(this).stopTime().oneTime(300, updateActlinkTable);
    });
    $("#execactlinkFilter").click(updateActlinkTable);
	/*
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
	*/

    initActlinkTable();
    if ($("#actlinkFilter").focus().val() != "" || bHasTypeFilter()) {
        updateActlinkTable();
    };

    $("#formLetterFilter a").click(function() {
        $("#actlinkFilter").val("^" + this.text);
        updateActlinkTable();
        return false;
    });
});
