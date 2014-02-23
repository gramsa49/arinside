function initFilterTable() {
    if (filterList != null) { 
		fltListObj = new FilterableTable('filterList', 'filterFilter', 'filterListFilterResultCount'); 
		fltListObj.onCreateHtmlRow(createFilterRowHtml);
	}
}

function updateFilterTable() {
    if (fltListObj != null) { fltListObj.filterTable(); }
}

function createFilterRowHtml(data) {
    return ($("<tr>")
        .append($("<td>")
            .append(getIcon(rootLevel, 5))
            .append($("<a>").attr("href", data[8]).text(data[0]))
        )
        .append($("<td>").text(AREnabled(data[1])))
        .append($("<td>").text(data[2]))
		.append($("<td>").text(data[3]))
		.append($("<td>").text(data[4]))
		.append($("<td>").text(data[5]))
		.append($("<td>").text(data[6]))
		.append($("<td>").text(data[7]))
    );
}

$('document').ready(function() {
    //var checkBoxes = $('#multiFilter input[type="checkbox"]');

    $(".clearable").on('propertychange keyup input paste', 'input.data_field', function(e) {
        if (e.keyCode == 27 /*Escape-Key*/) { $(this).val(''); }
        $(this).stopTime().oneTime(300, updateFilterTable);
    });
    $("#execfilterFilter").click(updateFilterTable);
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

    initFilterTable();
    if ($("#filterFilter").focus().val() != "" || fltListObj.hasTypeFilter()) {
        updateFilterTable();
    };

    $("#formLetterFilter a").click(function() {
        $("#filterFilter").val("^" + this.text);
        updateFilterTable();
        return false;
    });
});
