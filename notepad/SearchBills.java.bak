import com.portal.pcm.*;
import com.portal.pcm.fields.*;

public class SearchBills {

	
	private static boolean logout = true;
	class CustomOpcode {
		public static final int AIR_OP_SEARCH_BILLS = 100006;
	}
	public static void main(String[] args) {
		try {
			// Get the input flist
			FList inflist = createSearchFList();

			// Print input flist
			System.out.println("Input flist:");
			inflist.dump(); //alternate way to print

			// Create PCM connext necessary for connecting to the server. 
	 		// A valid Infranet.properties file should be in the classpath.
	 		// See context examples for additional information.
			PortalContext ctx = new PortalContext();
			ctx.connect();
			
			// Call the opcode
			FList searchoutflist = ctx.opcode(CustomOpcode.AIR_OP_SEARCH_BILLS, inflist);
			//FList searchoutflist = ctx.opcode(PortalOp.SEARCH, inflist);

			// Close PCM connection
			ctx.close(logout);

			// Print the return flist
			System.out.println("Output flist:");
			searchoutflist.dump(); // this is an alternate way to print out an flist

			System.out.println("Success!");

		} catch (EBufException ebuf) {
			System.out.println("You weren't able to call the PCM_OP_TEST_LOOPBACK opcode.");
			System.out.println(" * Do you have a correct Infranet.properties file in the classpath?");
			System.out.println(" * Is the Infranet server CM up?");
			System.out.println("Here's the error:");
			System.out.println(ebuf);
		}
	}
	
	/*******************************************************************

	 *******************************************************************/
	public static FList createSearchFList() throws EBufException {
		
		// create the search flist
		FList sFlist = new FList();
		
		//resflist, argsflist, sFlag, template
		FList resFlist = new FList();
		FList argsFlist = new FList();
		String template = "select X from /bill where F1 = V1 ";
		int sFlag = 256;
		
		//create search poid
		Poid sPoid = new Poid(1, -1, "/search");
		//sPoid.get();
		// add data to the flist
		Poid accPoid = new Poid(1, 678502, "/account");
		sFlist.set(FldPoid.getInst(), accPoid);
		sFlist.set(FldFlags.getInst(), sFlag);
		sFlist.set(FldTemplate.getInst(), template);
		
		//argument flist and adding it to main flist
		argsFlist.set(FldAccountObj.getInst(), accPoid);
		sFlist.setElement(FldArgs.getInst(), 1, argsFlist);
		
		//result flist 
		sFlist.setElement(FldResults.getInst(), 0, resFlist);
		
		return sFlist;
	}
}
