// Wait until the page loads
window.addEventListener("DOMContentLoaded", () => {
  const firstNidCell = document.querySelector(
    ".views-table tbody tr:first-child td.views-field-nid"
  );

  if (firstNidCell) {
    const firstNid = firstNidCell.textContent.trim();
    console.log("First NID in the table:", firstNid);
    console.log("First NID: " + firstNid); // or use it however you want

    setInterval(() => {
      fetch("http://82.180.137.10/api/v1/getlatestnodeid")
        .then((response) => {
          if (!response.ok) {
            throw new Error("Network response was not ok");
          }
          return response.json();
        })
        .then((data) => {
          if (data && data.nid !== undefined) {
            // refresh the page if not equal
            if (firstNid != data.nid) {
              window.location.reload();
            }
          } else {
            console.error("NID not found in response:", data);
          }
        })
        .catch((error) => {
          console.error("Fetch error:", error);
        });
    }, 2000);
  } else {
    console.log("No NID found in the first row");
  }
});
