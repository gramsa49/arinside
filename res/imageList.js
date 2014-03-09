function initImageTable() {
    if (imageList != null) { 
		imgListObj = new FilterableTable('imageList', 'imageFilter', 'imageListFilterResultCount'); 
		imgListObj.onCreateHtmlRow(createImageRowHtml);
	}
}

function updateImageTable() {
    if (imgListObj != null) { try { imgListObj.filterTable(); } catch (e) { document.write(e); } }
}

function createImageRowHtml(data) {
    return ($("<tr>")
        .append($("<td>")
            .append(getIcon(rootLevel, 17))
            .append($("<a>").attr("href", data[4]).text(data[0]))
        )
        .append($("<td>").text(AREnabled(data[1])))
        .append($("<td>").text(data[2]))
				.append($("<td>").text(data[3]))
    );
}

$('document').ready(function() {
    $(".clearable").on('propertychange keyup input paste', 'input.data_field', function(e) {
        if (e.keyCode == 27 /*Escape-Key*/) { $(this).val(''); }
        $(this).stopTime().oneTime(300, updateImageTable);
    });

    initImageTable();
    if ($("#imageFilter").focus().val() != "" || imgListObj.hasTypeFilter()) {
        updateImageTable();
    };

    $("#formLetterFilter a").click(function() {
        $("#imageFilter").val("^" + $(this).text());
        updateImageTable();
        return false;
    });
});
